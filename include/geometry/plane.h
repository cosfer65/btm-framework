#pragma once

#include "vector.h"

namespace btm {

    template <typename T>
    struct Plane {
        basevec3<T>  normal; // assumed normalized
        T d{ 0.0f }; // ax + by + cz + d = 0

        Plane() = default;

        Plane(const basevec3<T>& n, T dd) : normal(n), d(dd) {}

        static Plane fromPointNormal(const basepoint3<T>& p, const basevec3<T>& n) {
            basevec3<T> nn = normalize(n);
            T dd = -dot(nn, p - basepoint3<T>{ 0.0f, 0.0f, 0.0f });
            return Plane{ nn, dd };
        }
    };

    template <typename T>
    inline T signedDistance(const Plane<T>& plane, const basepoint3<T>& p) {
        return dot(plane.normal, p - basepoint3<T>{ 0.0f, 0.0f, 0.0f }) + plane.d;
    }

    template <typename T>
    inline basepoint3<T> project(const Plane<T>& plane, const basepoint3<T>& p) {
        T dist = signedDistance(plane, p);
        return p - plane.normal * dist;
    }

    template <typename T>
    inline bool isAbove(const Plane<T>& plane, const basepoint3<T>& p) {
        return signedDistance(plane, p) > 0.0f;
    }

    template <typename T>
    inline bool isBelow(const Plane<T>& plane, const basepoint3<T>& p) {
        return signedDistance(plane, p) < 0.0f;
    }

} // namespace btm
