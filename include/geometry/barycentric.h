#ifndef __barycentric_h__
#define __barycentric_h__

#include "vector.h"

namespace btm {
    template <typename T>
    struct barycentric_t {
        T u{ 0 };
        T v{ 0 };
        T w{ 0 };
    };

    template <typename T>
    inline barycentric_t<T> barycentric(const basepoint3<T>& p,
        const basepoint3<T>& a, const basepoint3<T>& b, const basepoint3<T>& c)
    {
        basevec3<T> v0 = b - a;
        basevec3<T> v1 = c - a;
        basevec3<T> v2 = p - a;

        T d00 = dot(v0, v0);
        T d01 = dot(v0, v1);
        T d11 = dot(v1, v1);
        T d20 = dot(v2, v0);
        T d21 = dot(v2, v1);

        T denom = d00 * d11 - d01 * d01;
        if (denom == 0) {
            return barycentric_t<T>{ 0, 0, 0 };
        }

        T v = (d11 * d20 - d01 * d21) / denom;
        T w = (d00 * d21 - d01 * d20) / denom;
        T u = 1 - v - w;

        return barycentric_t<T>{ u, v, w };
    }

    template <typename T>
    inline bool isInsideTriangle(const barycentric_t<T>& b) {
        return b.u >= 0 && b.v >= 0 && b.w >= 0 &&
            b.u <= 1 && b.v <= 1 && b.w <= 1;
    }
} // namespace btm

#endif // __barycentric_h__
