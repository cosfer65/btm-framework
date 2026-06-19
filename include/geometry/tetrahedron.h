#pragma once

#include "vector.h"
#include "barycentric.h"

namespace btm {

    template <typename T>
    struct Tetrahedron {
        basepoint3<T> a;
        basepoint3<T> b;
        basepoint3<T> c;
        basepoint3<T> d;
    };

    // -----------------------------------------------------------------------------
    // Signed volume (6x actual volume)
    // -----------------------------------------------------------------------------
    template <typename T>
    inline T signedVolume6(const Tetrahedron<T>& t) {
        basevec3<T> ad = t.a - t.d;
        basevec3<T> bd = t.b - t.d;
        basevec3<T> cd = t.c - t.d;
        return dot(ad, cross(bd, cd));
    }

    // -----------------------------------------------------------------------------
    // Actual volume
    // -----------------------------------------------------------------------------
    template <typename T>
    inline T volume(const Tetrahedron<T>& t) {
        return std::fabs(signedVolume6(t)) / 6.0f;
    }

    // -----------------------------------------------------------------------------
    // Orientation: positive if ABCD is positively oriented
    // -----------------------------------------------------------------------------
    template <typename T>
    inline T orient3D(const Tetrahedron<T>& t) {
        return signedVolume6(t);
    }

    // -----------------------------------------------------------------------------
    // Barycentric coordinates inside a tetrahedron
    // -----------------------------------------------------------------------------
    template <typename T>
    struct Barycentric4 {
        T w0; // weight for a
        T w1; // weight for b
        T w2; // weight for c
        T w3; // weight for d
    };

    template <typename T>
    inline Barycentric4<T> barycentric(const Tetrahedron<T>& t, const basepoint3<T>& p) {
        T v6 = signedVolume6(t);
        if (v6 == 0) {
            return Barycentric4<T>{ 0,0,0,0 };
        }

        Tetrahedron<T> t0{ p, t.b, t.c, t.d };
        Tetrahedron<T> t1{ t.a, p, t.c, t.d };
        Tetrahedron<T> t2{ t.a, t.b, p, t.d };
        Tetrahedron<T> t3{ t.a, t.b, t.c, p };

        T w0 = signedVolume6(t0) / v6;
        T w1 = signedVolume6(t1) / v6;
        T w2 = signedVolume6(t2) / v6;
        T w3 = signedVolume6(t3) / v6;

        return Barycentric4<T>{ w0, w1, w2, w3 };
    }

    // -----------------------------------------------------------------------------
    // Point-in-tetrahedron test
    // -----------------------------------------------------------------------------
    template <typename T>
    inline bool containsPoint(const Tetrahedron<T>& t, const basepoint3<T>& p) {
        Barycentric4<T> bc = barycentric(t, p);

        return bc.w0 >= 0.0f && bc.w1 >= 0.0f &&
            bc.w2 >= 0.0f && bc.w3 >= 0.0f &&
            bc.w0 <= 1.0f && bc.w1 <= 1.0f &&
            bc.w2 <= 1.0f && bc.w3 <= 1.0f;
    }

    // -----------------------------------------------------------------------------
    // Closest point on tetrahedron (useful for FEM, SDF, collision)
    // -----------------------------------------------------------------------------
    template <typename T>
    inline basepoint3<T> closestPoint(const Tetrahedron<T>& t, const basepoint3<T>& p) {
        // Compute barycentric coordinates
        Barycentric4<T> bc = barycentric(t, p);

        // If inside, return p projected into tetrahedron
        if (containsPoint(t, p)) {
            return p;
        }

        // Otherwise, clamp barycentric coordinates to the simplex
        T w0 = std::max(T(0), bc.w0);
        T w1 = std::max(T(0), bc.w1);
        T w2 = std::max(T(0), bc.w2);
        T w3 = std::max(T(0), bc.w3);

        T sum = w0 + w1 + w2 + w3;
        if (sum == T(0)) {
            // Degenerate tetrahedron: return closest vertex
            T da = distanceSquared(p, t.a);
            T db = distanceSquared(p, t.b);
            T dc = distanceSquared(p, t.c);
            T dd = distanceSquared(p, t.d);

            T m = std::min({ da, db, dc, dd });
            if (m == da) return t.a;
            if (m == db) return t.b;
            if (m == dc) return t.c;
            return t.d;
        }

        w0 /= sum;
        w1 /= sum;
        w2 /= sum;
        w3 /= sum;

        return basepoint3<T>{
            t.a.x() * w0 + t.b.x() * w1 + t.c.x() * w2 + t.d.x() * w3,
            t.a.y() * w0 + t.b.y() * w1 + t.c.y() * w2 + t.d.y() * w3,
            t.a.z() * w0 + t.b.z() * w1 + t.c.z() * w2 + t.d.z() * w3
        };
    }

} // namespace btm
