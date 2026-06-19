#pragma once

#include "tetrahedron.h"
#include "vector.h"

namespace btm {

    // N_i(p) = barycentric weight w_i
    template <typename T>
    inline Barycentric4<T> shapeFunctions(const Tetrahedron<T>& t, const basepoint3<T>& p) {
        return barycentric(t, p);
    }

    // Gradients of shape functions (constant per element)
    template <typename T>
    struct ShapeFunctionGradients {
        basevec3<T> gradN0;
        basevec3<T> gradN1;
        basevec3<T> gradN2;
        basevec3<T> gradN3;
    };

    template <typename T>
    inline ShapeFunctionGradients<T> shapeFunctionGradients(const Tetrahedron<T>& t) {
        // Using formula: grad N_i = (1 / (6V)) * n_i
        // where n_i is normal of opposite face with proper sign.
        T v6 = signedVolume6(t);
        if (v6 == 0) {
            return ShapeFunctionGradients<T>{
                basevec3<T>{0,0,0}, basevec3<T>{0,0,0}, basevec3<T>{0,0,0}, basevec3<T>{0,0,0}
            };
        }

        // Opposite faces:
        // N0 opposite face (b,c,d)
        basevec3<T> n0 = cross(t.c - t.b, t.d - t.b);
        // N1 opposite face (a,d,c)
        basevec3<T> n1 = cross(t.d - t.a, t.c - t.a);
        // N2 opposite face (a,b,d)
        basevec3<T> n2 = cross(t.b - t.a, t.d - t.a);
        // N3 opposite face (a,c,b)
        basevec3<T> n3 = cross(t.c - t.a, t.b - t.a);

        T inv6V = 1.0 / v6;

        ShapeFunctionGradients<T> g;
        g.gradN0 = n0 * inv6V;
        g.gradN1 = n1 * inv6V;
        g.gradN2 = n2 * inv6V;
        g.gradN3 = n3 * inv6V;
        return g;
    }

} // namespace btm
