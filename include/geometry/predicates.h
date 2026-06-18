#pragma once

#include "vector.h"
#include "triangle.h"

// Geometric predicates and orientation tests

namespace btm {
    // 2D orientation using x,y components
    template <typename T>
    inline T orient2D(const basepoint3<T>& a, const basepoint3<T>& b, const basepoint3<T>& c) {
        T ax = a.x(), ay = a.y();
        T bx = b.x(), by = b.y();
        T cx = c.x(), cy = c.y();
        return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
    }

    // 3D orientation (signed volume * 6)
    template <typename T>
    inline T orient3D(const basepoint3<T>& a,
        const basepoint3<T>& b,
        const basepoint3<T>& c,
        const basepoint3<T>& d)
    {
        basevec3<T> ad = a - d;
        basevec3<T> bd = b - d;
        basevec3<T> cd = c - d;
        return dot(ad, cross(bd, cd));
    }

    template <typename T>
    inline bool pointInTriangle(const basepoint3<T>& p,
        const basepoint3<T>& a,
        const basepoint3<T>& b,
        const basepoint3<T>& c)
    {
        barycentric_t<T> bc = barycentric(p, a, b, c);
        return isInsideTriangle(bc);
    }

    // Simple triangle-triangle intersection (not fully robust)
    template <typename T>
    inline bool triangleTriangleIntersect(const Triangle<T>& t0,
        const Triangle<T>& t1)
    {
        // Very minimal: check if any vertex of t0 is inside t1 and vice versa.
        if (containsPoint(t1, t0.a) ||
            containsPoint(t1, t0.b) ||
            containsPoint(t1, t0.c)) {
            return true;
        }
        if (containsPoint(t0, t1.a) ||
            containsPoint(t0, t1.b) ||
            containsPoint(t0, t1.c)) {
            return true;
        }
        // For Phase 1, this is enough; later you can add full segment-triangle tests.
        return false;
    }
} // namespace themesh
