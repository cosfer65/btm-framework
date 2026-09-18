#pragma once

#include <cmath>
#include <vector>

#ifdef _DEBUG
#include <iostream>
#endif

#include "base_definitions.h"

namespace btm {
    ///////////////////////////////////////////////////////////////////////////////////
    // matrices are row-major
    // assuming a 4x4 matrix, it is laid out as:
    //  0  4  8 12
    //  1  5  9 13
    //  2  6 10 14
    //  3  7 11 15
    //
    // make matrices column major to speed display with OpenGL,
    // which expects column major matrices. This is a tradeoff,
    // as it makes some operations less intuitive,
    // but it is more efficient for graphics applications.

    template <typename T, size_t ROWS, size_t COLS>
    struct basematrix {
    public:
        T data[ROWS * COLS]{ T(0) };

        int size_of() {
            return sizeof(data);
        }

        basematrix() {}

        basematrix(const basematrix<T, ROWS, COLS>& m) {
            memcpy(data, m.data, ROWS * COLS * sizeof(T));
        }

        basematrix(const T* data) {
            memcpy(this->data, data, ROWS * COLS * sizeof(T));
        }

        template <typename Q> requires std::is_arithmetic_v<Q>
        basematrix(Q initial_value) {
            for (int i = 0; i < ROWS * COLS; ++i)
                this->data[i] = T(initial_value);
        }

        basematrix(const std::vector<T>& init) {
            size_t i = 0;
            for (T v : init) {
                this->data[i] = v;
                ++i;
                // check to prevent out of bounds error
                if (i >= ROWS * COLS)
                    break;
            }
        }

        ~basematrix() {}

        size_t rows() const {
            return ROWS;
        }

        size_t cols() const {
            return COLS;
        }

        T& operator()(size_t row, size_t col) {
            return data[col * ROWS + row];
        }

        const T& operator()(size_t row, size_t col) const {
            return data[col * ROWS + row];
        }

        basematrix<T, ROWS, COLS>& operator=(const basematrix<T, ROWS, COLS>& m) {
            memcpy(data, m.data, ROWS * COLS * sizeof(T));
            return *this;
        }

        T& operator[](size_t index) {
            return data[index];
        }

        T operator[](size_t index) const {
            return data[index];
        }

        operator T* () {
            return this->data;
        }

        basematrix<T, ROWS, COLS>& operator+=(const basematrix<T, ROWS, COLS>& m) {
            for (int i = 0; i < ROWS * COLS; ++i)
                this->data[i] += m.data[i];
            return *this;
        }

        basematrix<T, ROWS, COLS>& operator-=(const basematrix<T, ROWS, COLS>& m) {
            for (int i = 0; i < ROWS * COLS; ++i)
                this->data[i] -= m.data[i];
            return *this;
        }

        basematrix<T, ROWS, COLS> operator-(void) {
            basematrix<T, ROWS, COLS> res(*this);
            for (int i = 0; i < ROWS * COLS; ++i)
                res.data[i] = -res.data[i];
            return res;
        }

        template <typename Q> requires std::is_arithmetic_v<Q>
        basematrix<T, ROWS, COLS>& scale(Q sc) {
            for (int i = 0; i < ROWS * COLS; ++i)
                this->data[i] *= T(sc);
            return *this;
        }

        template <typename Q> requires std::is_arithmetic_v<Q>
        basematrix<T, ROWS, COLS>& operator*=(Q v) {
            for (int i = 0; i < ROWS * COLS; ++i)
                this->data[i] *= T(v);
            return *this;
        }

        template <typename Q> requires std::is_arithmetic_v<Q>
        basematrix<T, ROWS, COLS>& operator/=(Q v) {
            for (int i = 0; i < ROWS * COLS; ++i)
                this->data[i] /= T(v);
            return *this;
        }

        void loadIdentity() {
            memset(this->data, 0, ROWS * COLS * sizeof(T));
            size_t D = std::min<size_t>(ROWS, COLS);
            for (size_t i = 0; i < D; ++i) {
                this->data[i * COLS + i] = T(1);
            }
        }

        void swap_cols(size_t c1, size_t c2) {
            T temp;
            for (size_t i = 0; i < ROWS; ++i) {
                temp = (*this)(i, c1);
                (*this)(i, c1) = (*this)(i, c2);
                (*this)(i, c2) = temp;
            }
        }

        void swap_rows(size_t r1, size_t r2) {
            T temp;
            for (size_t i = 0; i < COLS; ++i) {
                temp = (*this)(r1, i);
                (*this)(r1, i) = (*this)(r2, i);
                (*this)(r2, i) = temp;
            }
        }

        template <size_t R2, size_t C2> requires (COLS == R2)
            basematrix<T, ROWS, C2> operator*(const basematrix<T, R2, C2>& op2) const {
            basematrix<T, ROWS, C2> res;
            for (size_t r = 0; r < ROWS; ++r) {
                for (size_t c = 0; c < C2; ++c) {
                    res(r, c) = T(0);
                    for (size_t k = 0; k < R2; ++k) {
                        res(r, c) += (*this)(r, k) * op2(k, c);
                    }
                }
            }
            return res;
        }

        basematrix<T, COLS, ROWS> transpose() const {
            basematrix<T, COLS, ROWS> t;
            for (size_t r = 0; r < ROWS; ++r) {
                for (size_t c = 0; c < COLS; ++c) {
                    t(c, r) = (*this)(r, c);
                }
            }
            return t;
        }

        template <typename Q> requires std::is_arithmetic_v<Q>
        basematrix<T, ROWS, COLS> operator*(Q v) const {
            basematrix<T, ROWS, COLS> res;
            for (int i = 0; i < ROWS * COLS; ++i)
                res.data[i] = (*this)[i] * T(v);
            return res;
        }

        template <typename Q> requires std::is_arithmetic_v<Q>
        basematrix<T, ROWS, COLS> operator/(Q v) const {
            basematrix<T, ROWS, COLS> res;
            for (int i = 0; i < ROWS * COLS; ++i)
                res.data[i] = (*this)[i] / T(v);
            return res;
        }

        template <typename T, size_t ROWS, size_t COLS>
        basematrix<T, ROWS, COLS> operator+(const basematrix<T, ROWS, COLS>& op2) const {
            basematrix<T, ROWS, COLS> res(*this);
            for (int i = 0; i < ROWS * COLS; ++i)
                res.data[i] += op2.data[i];
            return res;
        }

        template <typename T, size_t ROWS, size_t COLS>
        basematrix<T, ROWS, COLS> operator-(const basematrix<T, ROWS, COLS>& op2) const {
            basematrix<T, ROWS, COLS> res(*this);
            for (int i = 0; i < ROWS * COLS; ++i)
                res.data[i] -= op2.data[i];
            return res;
        }

        bool operator==(const basematrix<T, ROWS, COLS>& B) const {
            for (size_t i = 0; i < ROWS * COLS; ++i) {
                if (T(fabs((*this)[i] - B[i])) > TOLLERANCE<T>)
                    return false;
            }
            return true;
        }

#ifdef _DEBUG
        void print(const std::string& label = "") const {
            for (size_t c = 0; c < COLS; ++c) {
                std::cout << "--";
            }
            std::cout << std::endl;
            std::cout << label;
            for (size_t r = 0; r < ROWS; ++r) {
                for (size_t c = 0; c < COLS; ++c) {
                    std::cout << data[r * COLS + c] << " ";
                }
                std::cout << std::endl;
            }
        }
        void print_by_cols(const std::string& label = "") const {
            std::cout << label;
            for (size_t c = 0; c < COLS; ++c) {
                for (size_t r = 0; r < ROWS; ++r) {
                    std::cout << data[r * COLS + c] << " ";
                }
                std::cout << std::endl;
            }
        }

        void print_line(const std::string& label = "") const {
            std::cout << label;
            for (size_t i = 0; i < ROWS * COLS; ++i) {
                std::cout << data[i] << " ";
            }
            std::cout << std::endl;
        }
#endif
    };

    template <typename T, size_t ROWS, size_t COLS, typename Q> requires std::is_arithmetic_v<Q>
    basematrix<T, ROWS, COLS> operator*(Q v, const basematrix<T, ROWS, COLS>& op1) {
        return op1 * v;
    }

#if 0
    template <typename T>
    bool invert3x3matrix(const basematrix<T, 3, 3>& m, basematrix<T, 3, 3>& invOut) {
        T det = m[0] * (m[4] * m[8] - m[5] * m[7])
            - m[1] * (m[3] * m[8] - m[5] * m[6])
            + m[2] * (m[3] * m[7] - m[4] * m[6]);

        if (det == 0) return false; // Singular matrix

        T invDet = T(1.0) / det;

        invOut[0] = (m[4] * m[8] - m[5] * m[7]) * invDet;
        invOut[1] = -(m[1] * m[8] - m[2] * m[7]) * invDet;
        invOut[2] = (m[1] * m[5] - m[2] * m[4]) * invDet;
        invOut[3] = -(m[3] * m[8] - m[5] * m[6]) * invDet;
        invOut[4] = (m[0] * m[8] - m[2] * m[6]) * invDet;
        invOut[5] = -(m[0] * m[5] - m[2] * m[3]) * invDet;
        invOut[6] = (m[3] * m[7] - m[4] * m[6]) * invDet;
        invOut[7] = -(m[0] * m[7] - m[1] * m[6]) * invDet;
        invOut[8] = (m[0] * m[4] - m[1] * m[3]) * invDet;

        return true;
    }

    template <typename M>
    concept MatrixLike =
        requires(M m) {
            { m.rows() } -> std::convertible_to<std::size_t>;
            { m.cols() } -> std::convertible_to<std::size_t>;
            { m(0, 0) };
    };

    template <typename T, MatrixLike M>
    bool invertMatrix(const M& A, M& inverse) {
        size_t n = A.rows();
        if (n == 0 || A.cols() != n)
            return false;
        // throw std::runtime_error("Matrix must be non-empty and square.");
        inverse = M();

        // Create augmented matrix [A | I]
        M aug;
        aug.resize(A.rows(), A.cols() * 2);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j)
                aug(i, j) = A(i, j);
            aug(i, n + i) = 1.0;
        }

        // Perform Gauss�Jordan elimination
        for (size_t col = 0; col < n; ++col) {
            // Find pivot
            size_t pivot = col;
            for (size_t row = col + 1; row < n; ++row)
                if (std::fabs(aug(row, col)) > std::fabs(aug(pivot, col)))
                    pivot = row;

            if (std::fabs(aug(pivot, col)) < 1e-12)
                return false; // Singular matrix

            // Swap pivot row into place
            aug.swap_rows(col, pivot);

            // Normalize pivot row
            T div = aug(col, col);
            for (size_t j = 0; j < 2 * n; ++j)
                aug(col, j) /= div;

            // Eliminate other rows
            for (size_t row = 0; row < n; ++row) {
                if (row == col) continue;
                T factor = aug(row, col);
                for (size_t j = 0; j < 2 * n; ++j)
                    aug(row, j) -= factor * aug(col, j);
            }
        }

        // Extract inverse matrix from augmented matrix
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                inverse(i, j) = aug(i, j + n);

        return true;
    }

    template <typename T, size_t RC>
    basematrix<T, RC, RC> getCofactor(const basematrix<T, RC + 1, RC + 1>& mat, size_t p, size_t q) {
        size_t n = mat.rows();
        basematrix<T, RC, RC> sub;

        size_t row = 0;
        for (size_t i = 0; i < n; i++) {
            if (i == p) continue;
            size_t col = 0;
            for (int j = 0; j < n; j++) {
                if (j == q) continue;
                sub(row, col) = mat(i, j);
                col++;
            }
            row++;
        }
        return sub;
    }

    template <typename T, size_t RC> requires (RC > 2)
        T determinant(const basematrix<T, RC, RC>& mat) {
        size_t n = mat.rows();
        if (n >= 3) {
            T det = 0.0;
            int sign = 1;

            for (size_t col = 0; col < n; col++) {
                basematrix<T, RC - 1, RC - 1> sub = getCofactor<T, RC - 1>(mat, 0, col);
                det += sign * mat(0, col) * determinant<float, RC - 1>(sub);
                sign = -sign; // alternate + -
            }
            return det;
        }
        return T(0);
    }

    template <typename T, size_t RC> requires (RC == 2)
        T determinant(const basematrix<T, RC, RC>& mat) {
        return mat(0, 0) * mat(1, 1) - mat(0, 1) * mat(1, 0);
    }

#endif

    template <typename T> using basemat3 = basematrix<T, 3, 3>;
    template <typename T> using basemat4 = basematrix<T, 4, 4>;

    using matrix4 = btm::basematrix<float, 4, 4>;

    using fmat2 = basematrix<float, 2, 2>;
    using fmat3 = basematrix<float, 3, 3>;
    using fmat4 = basematrix<float, 4, 4>;
    using fmat2x1 = basematrix<float, 2, 1>;
    using fmat1x2 = basematrix<float, 1, 2>;
    using fmat3x1 = basematrix<float, 3, 1>;
    using fmat1x3 = basematrix<float, 1, 3>;
    using fmat3x2 = basematrix<float, 3, 2>;
    using fmat2x3 = basematrix<float, 2, 3>;
    using fmat3x6 = basematrix<float, 3, 6>;
    using fmat6x3 = basematrix<float, 6, 3>;
    using fmat6x1 = basematrix<float, 6, 1>;
    using fmat2x2 = basematrix<float, 2, 2>;

    using dmat3 = basematrix<double, 3, 3>;
    using dmat4 = basematrix<double, 4, 4>;
    using dmat2x1 = basematrix<double, 2, 1>;
    using dmat1x2 = basematrix<double, 1, 2>;
    using dmat3x1 = basematrix<double, 3, 1>;
    using dmat1x3 = basematrix<double, 1, 3>;
    using dmat3x2 = basematrix<double, 3, 2>;
    using dmat2x3 = basematrix<double, 2, 3>;
    using dmat3x6 = basematrix<double, 3, 6>;
    using dmat6x3 = basematrix<double, 6, 3>;
    using dmat6x1 = basematrix<double, 6, 1>;
    using dmat2x2 = basematrix<double, 2, 2>;
}
