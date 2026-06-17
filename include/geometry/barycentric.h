#ifndef __barycentric_h__
#define __barycentric_h__

#include "vector.h"

namespace btm {

    template <typename T>
    struct Barycentric {
        T u{0};
        T v{0};
        T w{0};
    };

inline Barycentric3 barycentric(const Point3& p,
                                const Point3& a,
                                const Point3& b,
                                const Point3& c)
{
    Vec3 v0 = b - a;
    Vec3 v1 = c - a;
    Vec3 v2 = p - a;

    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;
    if (denom == 0.0f) {
        return Barycentric3{0.0f, 0.0f, 0.0f};
    }

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return Barycentric3{u, v, w};
}

inline bool isInsideTriangle(const Barycentric3& b) {
    return b.u >= 0.0f && b.v >= 0.0f && b.w >= 0.0f &&
           b.u <= 1.0f && b.v <= 1.0f && b.w <= 1.0f;
}

} // namespace btm


#endif // __barycentric_h__
