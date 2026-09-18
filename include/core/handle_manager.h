#pragma once

#include <vector>
#include <iostream>

#include "handle.h"

namespace btm {
    constexpr uint32_t INDEX_BITS = 20;
    constexpr uint32_t GEN_BITS = 12;

    constexpr uint32_t INDEX_MASK = (1u << INDEX_BITS) - 1u;
    constexpr uint32_t GEN_MASK = (1u << GEN_BITS) - 1u;

    inline uint32_t makeHandle(uint32_t index, uint32_t generation) {
        return (generation << INDEX_BITS) | index;
    }

    inline uint32_t handleIndex(uint32_t h) {
        return h & INDEX_MASK;
    }

    inline uint32_t handleGeneration(uint32_t h) {
        return (h >> INDEX_BITS) & GEN_MASK;
    }

    template<typename T>
    struct Slot {
        T resource;
        bool alive = false;
        uint32_t generation = 0;
        std::string debugName;
    };

    template<typename T>
    class HandleManager {
    public:
        using Handle = uint32_t;

        HandleManager() {

        }
        ~HandleManager() {
            cleanup();
            leakReport();
        }

        void cleanup()
        {
            for (auto& slot : slots)
                if (slot.alive) {
                    slot.resource.destroy(); // GPU free
                    slot.alive = false;
                }
        }

        GenericHandle create(auto&&... args)
        {
            uint32_t index = findFreeSlot();
            Slot<T>& slot = slots[index];

            slot.resource = T(std::forward<decltype(args)>(args)...);
            slot.resource.create(); // GPU allocation
            slot.alive = true;
            slot.generation++;

            GenericHandle gh;
            return GenericHandle(makeHandle(index, slot.generation));
        }

        T* get(GenericHandle gh)
        {
            uint32_t index = handleIndex(gh);
            uint32_t gen = handleGeneration(gh);

            if (index >= slots.size()) return nullptr;

            Slot<T>& slot = slots[index];
            if (!slot.alive || slot.generation != gen)
                return nullptr;

            return &slot.resource;
        }

        void destroy(GenericHandle gh)
        {
            uint32_t index = handleIndex(gh);
            uint32_t gen = handleGeneration(gh);

            if (index >= slots.size()) return;

            Slot<T>& slot = slots[index];
            if (slot.alive && slot.generation == gen) {
                slot.resource.destroy(); // GPU free
                slot.alive = false;
            }
        }

        void leakReport() const
        {
            for (size_t i = 0; i < slots.size(); ++i)
                if (slots[i].alive)
                    std::cout << "LEAK: slot " << i << " (" << slots[i].debugName << ")\n";
        }

    private:
        std::vector<Slot<T>> slots;

        uint32_t findFreeSlot()
        {
            uint32_t slotCount = static_cast<uint32_t>(slots.size());
            for (uint32_t i = 0; i < slotCount; ++i)
                if (!slots[i].alive)
                    return i;

            slots.emplace_back(); // Add a new slot
            return slots.size() - 1;
        }
    };
} // namespace btm
    