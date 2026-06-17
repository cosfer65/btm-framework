#ifndef __vector_h__
#define __vector_h__

#include "matrix.h"

namespace btm {
    /**
     * @brief A small, fixed-size N-component vector class template.
     *
     * Supports 2, 3 or 4 component vectors (compile-time C). Provides basic
     * arithmetic, length, normalization, dot/cross products and conversions.
     *
     * @tparam T Scalar type (e.g., float, double, int).
     * @tparam C Number of components (2, 3 or 4).
     */
    template <typename T, size_t C> class basevector {
        static_assert(C >= 2 && C <= 4,
            "basevector only supports 2, 3, or 4 components");
        T zero = T(0);

    public:
        /// Raw storage for components. Indexed as data[0]..data[C-1].
        T data[C] = { T(0) };

        /**
         * Component accessors.
         * Non-const returns reference allowing modification.
         * Const overloads return values.
         * For out-of-range components (e.g., z() on a 2-component vector)
         * a temporary zero is returned by reference for non-const or value for
         * const.
         */
        T& x() { return this->data[0]; }
        T& y() { return this->data[1]; }
        T& z() { return C > 2 ? this->data[2] : zero; }
        T& w() { return C > 3 ? this->data[3] : zero; }
        T x() const { return this->data[0]; }
        T y() const { return this->data[1]; }
        T z() const { return C > 2 ? this->data[2] : zero; }
        T w() const { return C > 3 ? this->data[3] : zero; }

        /// Default ctor: zero-initialized by the data member initializer.
        basevector() {}

        basevector(const std::vector<T>& init) {
            size_t i = 0;
            for (T v : init) {
                this->data[i] = v;
                ++i;
                // check to prevent out of bounds error
                if (i >= C)
                    break;
            }
        }

        /**
         * Construct from explicit components.
         * z_val and w_val are optional and only assigned if C allows.
         */
        basevector(T x_val, T y_val, T z_val = 0, T w_val = 0) {
            this->data[0] = x_val;
            this->data[1] = y_val;
            if (C > 2)
                this->data[2] = z_val;
            if (C > 3)
                this->data[3] = w_val;
        }

        /// Copy constructor
        basevector(const basevector<T, C>& initial) {
            for (size_t i = 0; i < C; ++i)
                this->data[i] = initial.data[i];
        }

        /// Fill constructor: set all components to `initial`.
        basevector(T initial) {
            for (size_t i = 0; i < C; ++i)
                this->data[i] = initial;
        }

        /**
         * Construct a C-component vector from a (C-1)-component vector plus one
         * value. Enabled only when C > 2 to avoid ambiguous 2-component
         * construction.
         */
        basevector(const basevector<T, C - 1>& iv, T _n)
            requires(C > 2)
        {
            for (size_t i = 0; i < C - 1; ++i)
                data[i] = iv.data[i];
            data[C - 1] = _n;
        }

        virtual ~basevector() {}

        /// Copy assignment
        basevector& operator=(const basevector& v) {
            for (size_t i = 0; i < C; ++i)
                this->data[i] = v.data[i];
            return *this;
        }

        /**
         * @brief Euclidean length (magnitude) of the vector.
         * @return length as T
         */
        T length() const {
            T len_sq = 0.0;
            for (size_t i = 0; i < C; ++i) {
                len_sq += this->data[i] * this->data[i];
            }
            return T(sqrt(len_sq));
        }

        /**
         * @brief Squared length (avoids sqrt).
         * @return squared length as T
         */
        T length_sq() const {
            T len_sq = 0.0;
            for (size_t i = 0; i < C; ++i) {
                len_sq += this->data[i] * this->data[i];
            }
            return len_sq;
        }

        /**
         * @brief Normalize the vector in-place.
         * If the length is very small (<= 1e-4) the vector is left unchanged.
         * @return reference to *this for chaining.
         */
        basevector& normalize() {
            double len = length();
            if (len > 0.0001) {
                len = 1.0 / len;
                for (size_t i = 0; i < C; ++i) {
                    this->data[i] = static_cast<T>(this->data[i] * len);
                }
            }
            return *this;
        }

        /**
         * @brief Normalize the vector in-place.
         * If the length is very small (<= 1e-4) the vector is left unchanged.
         * @return reference to *this for chaining.
         */
        basevector norm() {
            basevector n(*this);
            return n.normalize();
        }

        /// Number of components (compile-time constant).
        size_t size() const { return C; }

        /// Element access (mutable)
        T& operator()(size_t pos) { return this->data[pos]; }

        /**
         * Element access (const).
         * Throws std::out_of_range if pos >= C.
         */
        const T& operator()(size_t pos) const {
#ifdef _DEBUG
            if (pos >= C)
                throw std::out_of_range("basevector index out of range");
#endif
            return this->data[pos];
        }

        /**
         * @brief Dot product with another vector of same dimension.
         * @param v other vector
         * @return dot product (scalar)
         */
        T dot(const basevector<T, C>& v) const {
            T sum = 0;
            for (size_t i = 0; i < C; ++i) {
                sum += this->data[i] * v.data[i];
            }
            return sum;
        }

        /**
         * @brief Cross product. Defined only for 3-component vectors.
         * @param v other 3D vector
         * @return basevector<T,3> cross product
         */
        basevector<T, 3> cross(const basevector<T, C>& v) const {
            static_assert(C == 3, "Cross product is only defined for 3D vectors.");
            T cx = this->y() * v.z() - this->z() * v.y();
            T cy = this->z() * v.x() - this->x() * v.z();
            T cz = this->x() * v.y() - this->y() * v.x();
            return basevector<T, 3>(cx, cy, cz);
        }

        /// Cross product operator (vector * vector) for 3D vectors.
        basevector<T, 3> operator*(const basevector<T, 3>& v) const {
            return this->cross(v);
        }

        /// Vector addition
        basevector<T, C> operator+(const basevector<T, C>& v) const {
            basevector<T, C> res;
            for (size_t i = 0; i < C; ++i) {
                res.data[i] = this->data[i] + v.data[i];
            }
            return res;
        }

        /// In-place vector addition
        basevector<T, C>& operator+=(const basevector<T, C>& v) {
            for (size_t i = 0; i < C; ++i) {
                this->data[i] = this->data[i] + v.data[i];
            }
            return *this;
        }

        /// Vector subtraction
        basevector<T, C> operator-(const basevector<T, C>& v) const {
            basevector<T, C> res;
            for (size_t i = 0; i < C; ++i) {
                res.data[i] = this->data[i] - v.data[i];
            }
            return res;
        }

        /// Scalar division
        basevector<T, C> operator/(T v) const {
            basevector<T, C> res;
            for (size_t i = 0; i < C; ++i) {
                res.data[i] = this->data[i] / v;
            }
            return res;
        }

        /// Scalar multiplication
        basevector<T, C> operator*(T v) const {
            basevector<T, C> res;
            for (size_t i = 0; i < C; ++i) {
                res.data[i] = this->data[i] * v;
            }
            return res;
        }
        /// inplace Scalar multiplication
        basevector<T, C>& operator*=(T v) {
            for (size_t i = 0; i < C; ++i) {
                this->data[i] *= v;
            }
            return *this;
        }

        /// Negation operator. return Negated vector.
        basevector<T, C> operator-(void) {
            basevector<T, C> res(*this);
            for (int i = 0; i < C; ++i)
                res.data[i] = -res.data[i];
            return res;
        }

        /// Implicit conversion to raw pointer to components (T*).
        operator T* () { return this->data; }
    };

    template <typename T, size_t C>
    basevector<T, C> normalize(const basevector<T, C>& v) {
        basevector<T, C> res(v);
        return res.normalize();
    }

    template <typename T, size_t C>
    T dot(const basevector<T, C>& v1, const basevector<T, C>& v2) {
        return v1.dot(v2);
    }
    template <typename T, size_t C>
    basevector<T, C> cross(const basevector<T, C>& v1, const basevector<T, C>& v2) {
        return v1.cross(v2);
    }

    /**
     * @brief Matrix-vector multiplication: result = m * v
     * Generic for basematrix<T, R, C> and basevector<T, C>.
     *
     * @tparam T scalar type
     * @tparam R result vector dimension
     * @tparam C input vector dimension (and matrix columns)
     */
    template <typename T, size_t R, size_t C>
    basevector<T, R> operator*(const basematrix<T, R, C>& m,
        const basevector<T, C>& v) {
        basevector<T, R> mProduct;
        for (size_t i = 0; i < R; ++i) {
            mProduct[i] = T(0);
            for (size_t j = 0; j < C; ++j) {
                mProduct[i] += m.data[i * C + j] * v.data[j];
            }
        }
        return mProduct;
    }

#if 0
    template <typename T>
    basevector<T, 4><T> operator *(const basevector<T, 4><T>& v, const basematrix<T, 4, 4>& m) {
        basevector<T, 4><T> mProduct;
        mProduct[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
        mProduct[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
        mProduct[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
        mProduct[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];
        return mProduct;
    }
#endif

    /**
     * @brief Multiply a 4x4 matrix by a 3-component vector using only the
     * upper-left 3x3 portion. This is useful for rotating or scaling a 3D vector by
     * a transformation matrix without translation.
     */
    template <typename T>
    basevector<T, 3> operator*(const basematrix<T, 4, 4>& m,
        const basevector<T, 3>& v) {
        return basevector<T, 3>(m[0] * v.x() + m[1] * v.y() + m[2] * v.z() + m[3],
            m[4] * v.x() + m[5] * v.y() + m[6] * v.z() + m[7],
            m[8] * v.x() + m[9] * v.y() + m[10] * v.z() +
            m[11]);
    }

    template <typename T>
    basevector<T, 3> operator*(T s, const basevector<T, 3>& v) {
        return v * s;
    }

    /// Convenience wrapper to rotate a 3D vector with a 4x4 matrix (uses operator*
    /// above).
    template <typename T>
    basevector<T, 3> rotate_vector(const basematrix<T, 4, 4>& mMatrix,
        const basevector<T, 3>& vec) {
        return mMatrix * vec;
    }

    template <typename T>
    inline basevector<T, 3> project(const basevector<T, 3>& v, const basevector<T, 3>& onto) {
        T denom = onto.length_sq();
        if (denom == 0.0) {
            return basevector<T, 3>{ 0.0, 0.0, 0.0 };
        }
        T t = dot(v, onto) / denom;
        return onto * t;
    }

    template <typename T>
    inline basevector<T, 3> reject(const basevector<T, 3>& v, const basevector<T, 3>& from) {
        return v - project(v, from);
    }



    //////////////////////////////////////////////////////////////////////////

    template <typename T> using basevec2 = basevector<T, 2>;

    template <typename T> using basevec3 = basevector<T, 3>;

    template <typename T> using basevec4 = basevector<T, 4>;

    using ivec2 = basevec2<int>;
    using fvec3 = basevec3<float>;
    using fvec4 = basevec4<float>;
    using dvec3 = basevec3<double>;
    using dvec4 = basevec4<double>;

    // points are just vectors with a different name for semantic clarity.
    // They have the same structure and operations as vectors,
    // but we can use the point type to indicate when we are working with positions
    // rather than directions. This can help prevent confusion and bugs in code that
    // uses both points and vectors.
    template <typename T> using basepoint3 = basevec3<T>;

    using fpoint3 = basepoint3<float>;
    using dpoint3 = basepoint3<double>;
} // namespace btm

#endif // __vector_h__
