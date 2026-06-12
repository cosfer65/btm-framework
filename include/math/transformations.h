#ifndef __transformations_h__
#define __transformations_h__

#include "matrix.h"
#include "vector.h"

namespace btm {
    template <typename T>
    basemat4<T> Translation(T x, T y, T z) {
        basemat4<T> t;
        t.loadIdentity();
        t(0, 3) = x;
        t(1, 3) = y;
        t(2, 3) = z;
        return t;
    }

    template <typename T>
    basemat4<T> RotationX(T angle) {
        basemat4<T> r;
        r.loadIdentity();
        T c = cos(angle);
        T s = sin(angle);
        r(1, 1) = c;
        r(1, 2) = -s;
        r(2, 1) = s;
        r(2, 2) = c;
        return r;
    }
    template <typename T>
    basemat4<T> RotationY(T angle) {
        basemat4<T> r;
        r.loadIdentity();
        T c = cos(angle);
        T s = sin(angle);
        r(0, 0) = c;
        r(0, 2) = s;
        r(2, 0) = -s;
        r(2, 2) = c;
        return r;
    }
    template <typename T>
    basemat4<T> RotationZ(T angle) {
        basemat4<T> r;
        r.loadIdentity();
        T c = cos(angle);
        T s = sin(angle);
        r(0, 0) = c;
        r(0, 1) = -s;
        r(1, 0) = s;
        r(1, 1) = c;
        return r;
    }
    template <typename T>
    basemat4<T> Rotation(T angleX, T angleY, T angleZ) {
        basemat4<T> r = RotationZ(angleZ) * RotationY(angleY) * RotationX(angleX);
        return r;
    }

    template <typename T>
    basemat4<T> Scale(T sx, T sy, T sz) {
        basemat4<T> s;
        s.loadIdentity();
        s(0, 0) = sx;
        s(1, 1) = sy;
        s(2, 2) = sz;
        return s;
    }

    template <typename T>
    basemat4<T> LookAt(const basevec3<T>& eye, const basevec3<T>& center, const basevec3<T>& up) {
        basevec3<T> f = (center - eye);

        f = f * (1.0f / f.length());
        basevec3<T> s = cross(f, up);
        s = s * (1.0f / s.length());
        basevec3<T> u = cross(s, f);
        basemat4<T> result;
        result.loadIdentity();
        result(0, 0) = s.x();
        result(0, 1) = s.y();
        result(0, 2) = s.z();
        result(1, 0) = u.x();
        result(1, 1) = u.y();
        result(1, 2) = u.z();
        result(2, 0) = -f.x();
        result(2, 1) = -f.y();
        result(2, 2) = -f.z();
        result(0, 3) = -dot(s, eye);
        result(1, 3) = -dot(u, eye);
        result(2, 3) = dot(f, eye);
        return result;
    }

    template <typename T>
    basemat4<T> Perspective(T fov, T aspect, T nearPlane, T farPlane) {
        basemat4<T> result;
        result.loadIdentity();
        T tanHalfFOV = tan(fov / 2.0f);
        result(0, 0) = 1.0f / (aspect * tanHalfFOV);
        result(1, 1) = 1.0f / tanHalfFOV;
        result(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        result(3, 2) = -1.0f;
        result(3, 3) = 0.0f;
        return result;
    }

    template <typename T>
    void translate_matrix(basematrix<T, 4, 4>& matrix, const basevector<T, 3>& v) {
        matrix[3] =
            matrix[0] * v.x() + matrix[1] * v.y() + matrix[2] * v.z() + matrix[3];
        matrix[7] =
            matrix[4] * v.x() + matrix[5] * v.y() + matrix[6] * v.z() + matrix[7];
        matrix[11] =
            matrix[8] * v.x() + matrix[9] * v.y() + matrix[10] * v.z() + matrix[11];
        matrix[15] = matrix[12] * v.x() + matrix[13] * v.y() + matrix[14] * v.z() +
            matrix[15];
    }

    template <typename T> void remove_translation(basematrix<T, 4, 4>& m) {
        m[12] = m[13] = m[14] = m[3] = m[7] = m[11] = m[15] = T(0);
    }
}

#endif // __transformations_h__
