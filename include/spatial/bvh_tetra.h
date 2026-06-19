#pragma once

#include <vector>
#include <algorithm>
#include "bvh.h"
#include "tetrahedron.h"
#include "line.h"
#include "aabb.h"

#include "vector.h"

namespace btm {

    template <typename T>
    struct TetraHitInfo {
        T   t{ 0.0f };
        int     tetraIndex{ -1 };
        basepoint3<T>  position;
    };

    template <typename T>
    struct BVH_Tetra {
        std::vector<BVHNode<T>>    nodes;
        std::vector<Tetrahedron<T>> tets;
    };

    namespace detail {
        template <typename T>
        inline AABB<T> tetraBounds(const Tetrahedron<T>& t) {
            basepoint3<T> mi{
                std::min({t.a.x(), t.b.x(), t.c.x(), t.d.x()}),
                std::min({t.a.y(), t.b.y(), t.c.y(), t.d.y()}),
                std::min({t.a.z(), t.b.z(), t.c.z(), t.d.z()})
            };
            basepoint3<T> ma{
                std::max({t.a.x(), t.b.x(), t.c.x(), t.d.x()}),
                std::max({t.a.y(), t.b.y(), t.c.y(), t.d.y()}),
                std::max({t.a.z(), t.b.z(), t.c.z(), t.d.z()})
            };
            return AABB<T>{ mi, ma };
        }

        // reuse intersectAABB from BVH.hpp or copy it here
        template <typename T>
        inline bool intersectTetra(const Tetrahedron<T>& t, const ray<T>& _ray, T& tHit)
        {
            // Very simple: intersect ray with each face triangle and keep smallest t
            // Faces: (a,b,c), (a,b,d), (a,c,d), (b,c,d)
            T bestT = std::numeric_limits<T>::max();
            bool hit = false;

            auto testFace = [&](const Triangle<T>& tri) {
                T t, u, v;
                if (detail::intersectTriangle(tri, _ray, t, u, v)) {
                    if (t > 0.0f && t < bestT) {
                        bestT = t;
                        hit = true;
                    }
                }
                };

            testFace(Triangle<T>{ t.a, t.b, t.c });
            testFace(Triangle<T>{ t.a, t.b, t.d });
            testFace(Triangle<T>{ t.a, t.c, t.d });
            testFace(Triangle<T>{ t.b, t.c, t.d });

            if (hit) {
                tHit = bestT;
            }
            return hit;
        }

        template <typename T>
        inline int buildRecursive(BVH_Tetra<T>& bvh, std::vector<int>& indices, int start, int end)
        {
            int nodeIndex = static_cast<int>(bvh.nodes.size());
            bvh.nodes.emplace_back();
            BVHNode<T>& node = bvh.nodes.back();

            AABB<T> bounds = tetraBounds(bvh.tets[indices[start]]);
            for (int i = start + 1; i < end; ++i) {
                bounds = merge(bounds, tetraBounds(bvh.tets[indices[i]]));
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

            basevec3<T> extents{
                bounds.max_p.x() - bounds.min_p.x(),
                bounds.max_p.y() - bounds.min_p.y(),
                bounds.max_p.z() - bounds.min_p.z()
            };
            int axis = 0;
            if (extents.y() > extents.x() && extents.y() >= extents.z())
                axis = 1;
            else if (extents.z() > extents.x() && extents.z() >= extents.y()) 
                axis = 2;

            auto centerAxis = [&](int idx) {
                const Tetrahedron<T>& t = bvh.tets[idx];
                basepoint3<T> c{
                    (t.a.x() + t.b.x() + t.c.x() + t.d.x()) / 4.0f,
                    (t.a.y() + t.b.y() + t.c.y() + t.d.y()) / 4.0f,
                    (t.a.z() + t.b.z() + t.c.z() + t.d.z()) / 4.0f
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
    inline BVH_Tetra<T> build(const std::vector<Tetrahedron<T>>& tets) {
        BVH_Tetra<T> bvh;
        bvh.tets = tets;
        if (tets.empty()) return bvh;

        std::vector<int> indices(tets.size());
        for (int i = 0; i < static_cast<int>(tets.size()); ++i) {
            indices[i] = i;
        }

        detail::buildRecursive(bvh, indices, 0, static_cast<int>(indices.size()));

        std::vector<Tetrahedron<T>> reordered(tets.size());
        for (int i = 0; i < static_cast<int>(indices.size()); ++i) {
            reordered[i] = bvh.tets[indices[i]];
        }
        bvh.tets = std::move(reordered);

        return bvh;
    }

    template <typename T>
    inline bool intersect(const BVH_Tetra<T>& bvh, const ray<T>& _ray, TetraHitInfo<T>& hit)
    {
        if (bvh.nodes.empty()) return false;

        bool found = false;
        T closestT = std::numeric_limits<T>::max();
        int   closestIdx = -1;
        basepoint3<T> hitPos;

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
                    int tetIndex = node.start + i;
                    const Tetrahedron<T>& tet = bvh.tets[tetIndex];

                    T tHit;
                    if (detail::intersectTetra(tet, _ray, tHit)) {
                        if (tHit < closestT) {
                            closestT = tHit;
                            closestIdx = tetIndex;
                            found = true;
                            hitPos = _ray.origin + _ray.direction * tHit;
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
        hit.tetraIndex = closestIdx;
        hit.position = hitPos;
        return true;
    }

} // namespace themesh
