#pragma once

#include "line.h"
#include "triangle.h"
#include "barycentric.h"

namespace btm {

    template <typename T>
    inline basepoint3<T> closestPoint(const Segment<T>& s, const basepoint3<T>& p) {
        basevec3<T> ab = s.b - s.a;
        T abLenSq = lengthSquared(ab);
        if (abLenSq == 0.0f) {
            return s.a;
        }
        T t = dot(p - s.a, ab) / abLenSq;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return evaluate(s, t);
    }

    template <typename T>
    inline T distanceSquared(const Segment<T>& s, const basepoint3<T>& p) {
        basepoint3<T> cp = closestPoint(s, p);
        return distanceSquared(cp, p);
    }

    template <typename T>
    inline basepoint3<T> closestPoint(const Triangle<T>& tri, const basepoint3<T>& p) {
        // Ericson-style triangle closest point
        basevec3<T> ab = tri.b - tri.a;
        basevec3<T> ac = tri.c - tri.a;
        basevec3<T> ap = p - tri.a;

        T d1 = dot(ab, ap);
        T d2 = dot(ac, ap);
        if (d1 <= 0.0f && d2 <= 0.0f) return tri.a;

        basevec3<T> bp = p - tri.b;
        T d3 = dot(ab, bp);
        T d4 = dot(ac, bp);
        if (d3 >= 0.0f && d4 <= d3) return tri.b;

        T vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            T v = d1 / (d1 - d3);
            return tri.a + ab * v;
        }

        basevec3<T> cp = p - tri.c;
        T d5 = dot(ab, cp);
        T d6 = dot(ac, cp);
        if (d6 >= 0.0f && d5 <= d6) return tri.c;

        T vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            T w = d2 / (d2 - d6);
            return tri.a + ac * w;
        }

        T va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            T w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return tri.b + (tri.c - tri.b) * w;
        }

        // Inside face region
        barycentric_t<T> bc = barycentric(tri, p);
        return basepoint3<T>{
            tri.a.x * bc.u + tri.b.x * bc.v + tri.c.x * bc.w,
            tri.a.y * bc.u + tri.b.y * bc.v + tri.c.y * bc.w,
            tri.a.z * bc.u + tri.b.z * bc.v + tri.c.z * bc.w
        };
    }

    template <typename T>
    inline T distanceSquared(const Triangle<T>& tri, const basepoint3<T>& p) {
        basepoint3<T> cp = closestPoint(tri, p);
        return distanceSquared(cp, p);
    }

} // namespace themesh
