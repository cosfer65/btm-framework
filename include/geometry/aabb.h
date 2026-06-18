#pragma once

#include "vector.h"

// Axis-Aligned Bounding Box (AABB) structure and related functions

namespace btm {
    template <typename T>
    struct AABB {
        basepoint3<T> min_p;
        basepoint3<T> max_p;

        AABB()
            : min_p{ 0.0f, 0.0f, 0.0f }
            , max_p{ 0.0f, 0.0f, 0.0f }
        {}

        AABB(const basepoint3<T>& mi, const basepoint3<T>& ma)
            : min_p(mi), max_p(ma)
        {}
    };

    template <typename T>
    inline AABB<T> merge(const AABB<T>& a, const AABB<T>& b) {
        basepoint3<T> mi{
            std::min(a.min_p.x(), b.min_p.x()),
            std::min(a.min_p.y(), b.min_p.y()),
            std::min(a.min_p.z(), b.min_p.z())
        };
        basepoint3<T> ma{
            std::max(a.max_p.x(), b.max_p.x()),
            std::max(a.max_p.y(), b.max_p.y()),
            std::max(a.max_p.z(), b.max_p.z())
        };
        return AABB<T>{ mi, ma };
    }

    template <typename T>
    inline bool intersects(const AABB<T>& a, const AABB<T>& b) {
        if (a.max_p.x() < b.min_p.x() || a.min_p.x() > b.max_p.x()) return false;
        if (a.max_p.y() < b.min_p.y() || a.min_p.y() > b.max_p.y()) return false;
        if (a.max_p.z() < b.min_p.z() || a.min_p.z() > b.max_p.z()) return false;
        return true;
    }

    template <typename T>
    inline void expand(AABB<T>& box, T amount) {
        box.min_p.x() -= amount;
        box.min_p.y() -= amount;
        box.min_p.z() -= amount;
        box.max_p.x() += amount;
        box.max_p.y() += amount;
        box.max_p.z += amount;
    }

    template <typename T>
    inline T surfaceArea(const AABB<T>& box) {
        basevec3<T> extents{
            box.max_p.x() - box.min_p.x(),
            box.max_p.y() - box.min_p.y(),
            box.max_p.z() - box.min_p.z()
        };
        return 2.0f * (extents.x() * extents.y() +
            extents.x() * extents.z() +
            extents.y() * extents.z());
    }

    template <typename T>
    inline basepoint3<T> center(const AABB<T>& box) {
        return basepoint3<T>{
            0.5f * (box.min_p.x() + box.max_p.x()),
                0.5f * (box.min_p.y() + box.max_p.y()),
                0.5f * (box.min_p.z() + box.max_p.z())
        };
    }
} // namespace btm
