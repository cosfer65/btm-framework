#pragma once

namespace btm {

    /////////////////////////////////////////////////////////////
    /**
     * @brief Computes eigenvalues and eigenvectors of a 2x2 symmetric matrix.
     *
     * Given a symmetric matrix S = [a b; b c], this function computes its eigenvalues
     * and corresponding eigenvectors using the characteristic polynomial approach.
     *
     * @param a Element (0,0) of the matrix
     * @param b Off-diagonal element (0,1) and (1,0)
     * @param c Element (1,1) of the matrix
     * @param[out] l1 First eigenvalue (larger)
     * @param[out] l2 Second eigenvalue (smaller)
     * @param[out] v1x X-component of first eigenvector
     * @param[out] v1y Y-component of first eigenvector
     * @param[out] v2x X-component of second eigenvector
     * @param[out] v2y Y-component of second eigenvector
     *
     * @note The eigenvectors are normalized and orthogonal.
     */
    template <typename T>
    void eigenSymmetric2x2(T a, T b, T c,
                            T& l1, T& l2,
                            T& v1x, T& v1y,
                            T& v2x, T& v2y)
    {
        // trace and determinant
        T tr = a + c;
        T det = a * c - b * b;
        T disc = tr * tr - 4.0 * det;
        if (disc < 0.0) disc = 0.0;
        T s = std::sqrt(disc);

        l1 = 0.5 * (tr + s);
        l2 = 0.5 * (tr - s);

        // eigenvector for l1
        if (std::fabs(b) > 1e-12) {
            v1x = l1 - c;
            v1y = b;
        }
        else {
            // matrix is diagonal or nearly
            v1x = 1.0;
            v1y = 0.0;
        }
        T n1 = std::sqrt(v1x * v1x + v1y * v1y);
        if (n1 > 0.0) { v1x /= n1; v1y /= n1; }

        // eigenvector for l2: orthogonal to v1
        v2x = -v1y;
        v2y = v1x;
    }




}