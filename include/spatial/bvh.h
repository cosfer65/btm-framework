#pragma once

#include <vector>
#include <algorithm>
#include "triangle.h"
#include "line.h"

// Bounding Volume Hierarchy

namespace btm {
    template <typename T>
    struct BVHNode {
        AABB<T> bounds;
        int  leftChild{ -1 };
        int  rightChild{ -1 };
        int  start{ 0 };  // index into primitive array
        int  count{ 0 };  // number of primitives

        bool isLeaf() const {
            return count > 0;
        }
    };

    template <typename T>
    struct HitInfo {
        T   t{ 0.0f };
        int     triangleIndex{ -1 };
        basepoint3<T>  position;
        basevec3<T>    normal;
    };

    template <typename T>
    struct BVH {
        std::vector<BVHNode<T>>   nodes;
        std::vector<Triangle<T>>  triangles;
    };

    namespace detail {
        template <typename T>
        inline AABB<T> triangleBounds(const Triangle<T>& tri) {
            basepoint3<T> mi{
                std::min({tri.a.x(), tri.b.x(), tri.c.x()}),
                std::min({tri.a.y(), tri.b.y(), tri.c.y()}),
                std::min({tri.a.z(), tri.b.z(), tri.c.z()})
            };
            basepoint3<T> ma{
                std::max({tri.a.x(), tri.b.x(), tri.c.x()}),
                std::max({tri.a.y(), tri.b.y(), tri.c.y()}),
                std::max({tri.a.z(), tri.b.z(), tri.c.z()})
            };
            return AABB<T>{ mi, ma };
        }

        template <typename T>
        inline bool intersectAABB(const AABB<T>& box, const ray<T>& _ray, T& tmin, T& tmax) {
            tmin = 0.0f;
            tmax = std::numeric_limits<T>::max();

            for (int i = 0; i < 3; ++i) {
                T origin = (i == 0 ? _ray.origin.x() : (i == 1 ? _ray.origin.y() : _ray.origin.z()));
                T dir = (i == 0 ? _ray.direction.x() : (i == 1 ? _ray.direction.y() : _ray.direction.z()));
                T minB = (i == 0 ? box.min_p.x() : (i == 1 ? box.min_p.y() : box.min_p.z()));
                T maxB = (i == 0 ? box.max_p.x() : (i == 1 ? box.max_p.y() : box.max_p.z()));

                if (dir == 0.0f) {
                    if (origin < minB || origin > maxB) return false;
                }
                else {
                    T invD = 1.0f / dir;
                    T t0 = (minB - origin) * invD;
                    T t1 = (maxB - origin) * invD;
                    if (t0 > t1) std::swap(t0, t1);
                    tmin = std::max(tmin, t0);
                    tmax = std::min(tmax, t1);
                    if (tmax < tmin) return false;
                }
            }
            return true;
        }

        // Simple ray-triangle intersection (Möller–Trumbore)
        template <typename T>
        inline bool intersectTriangle(const Triangle<T>& tri,
            const ray<T>& _ray,
            T& t,
            T& u,
            T& v)
        {
            const T EPS = static_cast<T>(1e-6);
            basevec3<T> edge1 = tri.b - tri.a;
            basevec3<T> edge2 = tri.c - tri.a;
            basevec3<T> pvec = cross(_ray.direction, edge2);
            T det = dot(edge1, pvec);

            if (std::fabs(det) < EPS) return false;
            T invDet = 1.0f / det;

            basevec3<T> tvec = _ray.origin - tri.a;
            u = dot(tvec, pvec) * invDet;
            if (u < 0.0f || u > 1.0f) return false;

            basevec3<T> qvec = cross(tvec, edge1);
            v = dot(_ray.direction, qvec) * invDet;
            if (v < 0.0f || u + v > 1.0f) return false;

            t = dot(edge2, qvec) * invDet;
            if (t < 0.0f) return false;

            return true;
        }

        template <typename T>
        inline int buildRecursive(BVH<T>& bvh,
            std::vector<int>& indices,
            int start,
            int end)
        {
            int nodeIndex = static_cast<int>(bvh.nodes.size());
            bvh.nodes.emplace_back();
            BVHNode<T>& node = bvh.nodes.back();

            // Compute bounds
            AABB<T> bounds = triangleBounds(bvh.triangles[indices[start]]);
            for (int i = start + 1; i < end; ++i) {
                bounds = merge(bounds, triangleBounds(bvh.triangles[indices[i]]));
            }
            node.bounds = bounds;

            int count = end - start;
            if (count <= 4) {
                node.start = start;
                node.count = count;
                node.leftChild = -1;
                node.rightChild = -1;
                return nodeIndex;
            }

            // Choose split axis by largest extent
            basevec3<T> extents{
                bounds.max_p.x() - bounds.min_p.x(),
                bounds.max_p.y() - bounds.min_p.y(),
                bounds.max_p.z() - bounds.min_p.z()
            };
            int axis = 0;
            if (extents.y() > extents.x() && extents.y() >= extents.z()) axis = 1;
            else if (extents.z() > extents.x() && extents.z() >= extents.y()) axis = 2;

            auto centerAxis = [&](int idx) {
                const Triangle<T>& tri = bvh.triangles[idx];
                basepoint3<T> c{
                    (tri.a.x() + tri.b.x() + tri.c.x()) / 3.0f,
                    (tri.a.y() + tri.b.y() + tri.c.y()) / 3.0f,
                    (tri.a.z() + tri.b.z() + tri.c.z()) / 3.0f
                };
                return (axis == 0 ? c.x() : (axis == 1 ? c.y() : c.z()));
                };

            int mid = (start + end) / 2;
            std::nth_element(indices.begin() + start,
                indices.begin() + mid,
                indices.begin() + end,
                [&](int lhs, int rhs) {
                    return centerAxis(lhs) < centerAxis(rhs);
                });

            node.leftChild = buildRecursive(bvh, indices, start, mid);
            node.rightChild = buildRecursive(bvh, indices, mid, end);
            node.start = -1;
            node.count = 0;

            return nodeIndex;
        }

    } // namespace detail

    template <typename T>
    inline BVH<T> build(const std::vector<Triangle<T>>& tris) {
        BVH<T> bvh;
        bvh.triangles = tris;

        if (tris.empty()) return bvh;

        std::vector<int> indices(tris.size());
        for (int i = 0; i < static_cast<int>(tris.size()); ++i) {
            indices[i] = i;
        }

        detail::buildRecursive(bvh, indices, 0, static_cast<int>(indices.size()));

        // Reorder triangles according to indices
        std::vector<Triangle<T>> reordered(tris.size());
        for (int i = 0; i < static_cast<int>(indices.size()); ++i) {
            reordered[i] = bvh.triangles[indices[i]];
        }
        bvh.triangles = std::move(reordered);

        return bvh;
    }

    template <typename T>
    inline bool intersect(const BVH<T>& bvh,
        const ray<T>& _ray,
        HitInfo<T>& hit)
    {
        if (bvh.nodes.empty()) return false;

        bool found = false;
        T closestT = std::numeric_limits<T>::max();
        int   closestIdx = -1;
        basepoint3<T> hitPos;
        basevec3<T>   hitNormal;

        std::vector<int> stack;
        stack.reserve(bvh.nodes.size());
        stack.push_back(0);

        while (!stack.empty()) {
            int nodeIndex = stack.back();
            stack.pop_back();

            const BVHNode<T>& node = bvh.nodes[nodeIndex];
            T tmin, tmax;
            if (!detail::intersectAABB(node.bounds, _ray, tmin, tmax)) {
                continue;
            }

            if (node.isLeaf()) {
                for (int i = 0; i < node.count; ++i) {
                    int triIndex = node.start + i;
                    const Triangle<T>& tri = bvh.triangles[triIndex];

                    T t, u, v;
                    if (detail::intersectTriangle(tri, _ray, t, u, v)) {
                        if (t < closestT) {
                            closestT = t;
                            closestIdx = triIndex;
                            found = true;

                            hitPos = _ray.origin + _ray.direction * t;
                            hitNormal = normal(tri);
                        }
                    }
                }
            }
            else {
                if (node.leftChild >= 0) stack.push_back(node.leftChild);
                if (node.rightChild >= 0) stack.push_back(node.rightChild);
            }
        }

        if (!found) return false;

        hit.t = closestT;
        hit.triangleIndex = closestIdx;
        hit.position = hitPos;
        hit.normal = hitNormal;
        return true;
    }

    template <typename T>
    inline void refit(BVH<T>& bvh) {
        // Simple bottom-up refit
        for (int i = static_cast<int>(bvh.nodes.size()) - 1; i >= 0; --i) {
            BVHNode<T>& node = bvh.nodes[i];
            if (node.isLeaf()) {
                AABB<T> bounds = detail::triangleBounds(bvh.triangles[node.start]);
                for (int j = 1; j < node.count; ++j) {
                    bounds = merge(bounds, detail::triangleBounds(bvh.triangles[node.start + j]));
                }
                node.bounds = bounds;
            }
            else {
                AABB<T> bounds = bvh.nodes[node.leftChild].bounds;
                bounds = merge(bounds, bvh.nodes[node.rightChild].bounds);
                node.bounds = bounds;
            }
        }
    }

} // namespace themesh
