#pragma once

#include "vector.h"

namespace btm {

    template <typename T>
    struct AABB {
        basepoint3<T> min;
        basepoint3<T> max;

        AABB()
            : min{ 0.0f, 0.0f, 0.0f }
            , max{ 0.0f, 0.0f, 0.0f }
        {}

        AABB(const basepoint3<T>& mi, const basepoint3<T>& ma)
            : min(mi), max(ma)
        {}
    };

    template <typename T>
    inline AABB<T> merge(const AABB<T>& a, const AABB<T>& b) {
        basepoint3<T> mi{
            std::min(a.min.x, b.min.x),
            std::min(a.min.y, b.min.y),
            std::min(a.min.z, b.min.z)
        };
        basepoint3<T> ma{
            std::max(a.max.x, b.max.x),
            std::max(a.max.y, b.max.y),
            std::max(a.max.z, b.max.z)
        };
        return AABB<T>{ mi, ma };
    }

    template <typename T>
    inline bool intersects(const AABB<T>& a, const AABB<T>& b) {
        if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
        if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
        if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
        return true;
    }

    template <typename T>
    inline void expand(AABB<T>& box, T amount) {
        box.min.x -= amount;
        box.min.y -= amount;
        box.min.z -= amount;
        box.max.x += amount;
        box.max.y += amount;
        box.max.z += amount;
    }

    template <typename T>
    inline T surfaceArea(const AABB<T>& box) {
        basevec3<T> extents{

            box.max.x - box.min.x,
            box.max.y - box.min.y,
            box.max.z - box.min.z
        };
        return 2.0f * (extents.x * extents.y +
            extents.x * extents.z +
            extents.y * extents.z);
    }

    template <typename T>
    inline basepoint3<T> center(const AABB<T>& box) {
        return basepoint3<T>{
            0.5f * (box.min.x + box.max.x),
            0.5f * (box.min.y + box.max.y),
            0.5f * (box.min.z + box.max.z)
        };
    }

} // namespace btm
