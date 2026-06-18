#pragma once

#include <vector>
#include "vector.h"

namespace btm
{
    namespace geometry
    {
        //----------------------
        // Core data structures
        //----------------------

        template <typename T>
        struct SurfacePoint
        {
            basepoint3<T> x; // position
            basevec3<T> xu;  // ∂x/∂u
            basevec3<T> xv;  // ∂x/∂v
            basevec3<T> n;   // unit normal
        };

        template <typename T>
        struct MetricTensor
        {
            T E, F, G;
        };

        template <typename T>
        struct Curvatures
        {
            T H; // mean curvature
            T K; // Gaussian curvature
        };

        template <typename T>
        struct CurvatureTensor
        {
            T k1, k2;
            basevec3<T> d1, d2; // principal directions
        };

        template <typename T>
        CurvatureTensor<T> computeCurvature(
            const SurfacePoint<T>& p,
            const basevec3<T>& xuu,
            const basevec3<T>& xuv,
            const basevec3<T>& xvv)
        {
            // Build first and second fundamental forms
            T E = dot(p.xu, p.xu);
            T F = dot(p.xu, p.xv);
            T G = dot(p.xv, p.xv);

            T L = dot(xuu, p.n);
            T M = dot(xuv, p.n);
            T N = dot(xvv, p.n);
            // Solve generalized eigenproblem |II - k I| = 0
            // (implementation omitted for brevity)

            CurvatureTensor<T> K;
            // fill K.k1, K.k2, K.d1, K.d2
            return K;
        }

        template <typename T>
        struct TangentFrame
        {
            basevec3<T> t1, t2, n;
        };

        //----------------------
        // Parametric surfaces
        //----------------------
        template <typename T>
        inline MetricTensor<T> computeMetric(const SurfacePoint<T>& p)
        {
            MetricTensor<T> m;
            m.E = dot(p.xu, p.xu);
            m.F = dot(p.xu, p.xv);
            m.G = dot(p.xv, p.xv);
            return m;
        }

        template <typename T>
        inline basevec3<T> computeNormal(const SurfacePoint<T>& p)
        {
            return normalize(cross(p.xu, p.xv));
        }

        template <typename T>
        inline Curvatures<T> computeHK(
            const SurfacePoint<T>& p,
            const basevec3<T>& xuu,
            const basevec3<T>& xuv,
            const basevec3<T>& xvv)
        {
            MetricTensor<T> m = computeMetric(p);

            T L = dot(xuu, p.n);
            T M = dot(xuv, p.n);
            T N = dot(xvv, p.n);

            T denom = (m.E * m.G - m.F * m.F);

            Curvatures<T> c;
            c.H = (m.G * L - 2.0 * m.F * M + m.E * N) / (2.0 * denom);
            c.K = (L * N - M * M) / denom;
            return c;
        }

        template <typename T>
        inline basematrix<T, 2, 2> shapeOperator(
            const SurfacePoint<T>& p,
            const basevec3<T>& xuu,
            const basevec3<T>& xuv,
            const basevec3<T>& xvv)
        {
            MetricTensor<T> m = computeMetric(p);

            T L = dot(xuu, p.n);
            T M = dot(xuv, p.n);
            T N = dot(xvv, p.n);

            // fmat2 I  = { {m.E, m.F}, {m.F, m.G} };
            // fmat2 II = { {L,   M  }, {M,   N  } };
            basematrix<T, 2, 2> I({ m.E, m.F, m.F, m.G });
            basematrix<T, 2, 2> II({ L, M, M, N });

            return inverse(I) * II;
        }

        template <typename T>
        inline TangentFrame<T> buildFrame(const SurfacePoint<T>& p)
        {
            TangentFrame<T> f;
            f.t1 = normalize(p.xu);
            f.n = normalize(cross(p.xu, p.xv));
            f.t2 = normalize(cross(f.n, f.t1));
            return f;
        }

        //----------------------
        // Discrete mesh hooks
        //----------------------

        template <typename T>
        struct Mesh
        {
            // You’ll wire these to your actual mesh type.
            int vertexCount() const;

            basepoint3<T> position(int v) const;
            void setPosition(int v, const basepoint3<T>& p);

            std::vector<int> facesAroundVertex(int v) const;
            std::vector<int> edgesAroundVertex(int v) const;
            std::vector<int> facesAroundEdge(int v0, int v1) const;

            basevec3<T> faceNormal(int f) const;
            T faceArea(int f) const;
            T cornerAngle(int f, int v) const;

            int thirdVertexOfFace(int f, int v0, int v1) const;
            int otherVertex(int e, int v) const;
        };

        template <typename T>
        basevec3<T> computeVertexNormal(int v, const Mesh<T>& mesh)
        {
            basevec3<T> n(0.0);
            for (auto f : mesh.facesAroundVertex(v))
            {
                basevec3<T> fn = mesh.faceNormal(f);
                T angle = mesh.cornerAngle(f, v);
                n += angle * fn;
            }
            return normalize(n);
        }

        template <typename T>
        inline basevec3<T> vertexNormalAngleWeighted(int v, const Mesh<T>& mesh)
        {
            basevec3<T> n(0.0f);
            for (auto f : mesh.facesAroundVertex(v))
            {
                basevec3<T> fn = mesh.faceNormal(f);
                T angle = mesh.cornerAngle(f, v);
                n += angle * fn;
            }
            return normalize(n);
        }

        template <typename T>
        inline basevec3<T> meanCurvatureNormal(int v, const Mesh<T>& mesh)
        {
            basevec3<T> sum(0.0f);
            T area = 0.0;

            for (auto f : mesh.facesAroundVertex(v))
            {
                T A = mesh.faceArea(f);
                area += A;
                basevec3<T> n = mesh.faceNormal(f);
                sum += A * n;
            }

            return sum / (2.0 * area);
        }

        template <typename T>
        inline T cotangentWeight(const Mesh<T>& mesh, int v0, int v1)
        {
            auto faces = mesh.facesAroundEdge(v0, v1);
            T w = 0.0;

            for (auto f : faces)
            {
                int v2 = mesh.thirdVertexOfFace(f, v0, v1);

                basevec3<T> a = mesh.position(v0) - mesh.position(v2);
                basevec3<T> b = mesh.position(v1) - mesh.position(v2);

                T cosA = dot(a, b);
                T sinA = length(cross(a, b));

                w += cosA / sinA;
            }

            return 0.5 * w;
        }

        template <typename T>
        inline basevec3<T> laplacian(int v, const Mesh<T>& mesh)
        {
            basevec3<T> sum(0.0f);
            T wsum = 0.0;

            for (auto e : mesh.edgesAroundVertex(v))
            {
                int j = mesh.otherVertex(e, v);
                T w = cotangentWeight(mesh, v, j);
                sum += w * (mesh.position(j) - mesh.position(v));
                wsum += w;
            }

            return (wsum > 0.0) ? sum / wsum : basevec3<T>(0.0f);
        }

        template <typename T>
        inline void smoothMesh(Mesh<T>& mesh, T timestep)
        {
            std::vector<basevec3<T>> newPos(mesh.vertexCount());

            for (int v = 0; v < mesh.vertexCount(); ++v)
            {
                basevec3<T> lap = laplacian(v, mesh);
                newPos[v] = mesh.position(v) + timestep * lap;
            }

            for (int v = 0; v < mesh.vertexCount(); ++v)
                mesh.setPosition(v, newPos[v]);
        }

        template <typename T>
        inline T refinementMetric(const Curvatures<T>& c, T targetSize)
        {
            T k = std::max(std::abs(c.H), std::abs(c.K));
            return targetSize / (1.0 + targetSize * k);
        }
    } // namespace geometry
} // namespace btm
