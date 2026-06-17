#pragma once

#include "vector.h"
#include "barycentric.h"

namespace btm {

    template <typename T>
    struct Triangle {
        basepoint3<T> a;
        basepoint3<T> b;
        basepoint3<T> c;
    };

    template <typename T>
    inline basevec3<T> normal(const Triangle<T>& tri) {
        basevec3<T> ab = tri.b - tri.a;
        basevec3<T> ac = tri.c - tri.a;
        return normalize(cross(ab, ac));
    }

    template <typename T>
    inline T area(const Triangle<T>& tri) {
        basevec3<T> ab = tri.b - tri.a;
        basevec3<T> ac = tri.c - tri.a;
        return 0.5f * length(cross(ab, ac));
    }

    template <typename T>
    inline barycentric_t<T> barycentric(const Triangle<T>& tri, const basepoint3<T>& p) {
        return barycentric(p, tri.a, tri.b, tri.c);
    }

    template <typename T>
    inline bool containsPoint(const Triangle<T>& tri, const basepoint3<T>& p) {
        barycentric_t<T> b = barycentric(tri, p);
        return isInsideTriangle(b);
    }

    // template <typename T>
    // inline basepoint3<T> closestPoint(const Triangle<T>& tri, const basepoint3<T>& p);

} // namespace themesh
