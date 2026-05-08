#ifndef __mesh_explicit_h__
#define __mesh_explicit_h__


#include <vector>
#include <cstdint>
#include "vector.h"

namespace btm_framework
{
    template <typename T>
    struct Vertex
    {
        basevector<T, 3> position;
    };

    struct Triangle
    {
        std::uint32_t v0, v1, v2;
    };

    struct VertexAdjacency
    {
        std::vector<std::uint32_t> incident_faces;
        std::vector<std::uint32_t> neighbor_vertices;
    };

    template <typename T>
    struct MeshAttributes
    {
        std::vector<basevector<T, 3>> vertex_normals;

        // Curvature fields
        std::vector<T> k1;  // principal curvature 1
        std::vector<T> k2;  // principal curvature 2

        // Segmentation labels
        std::vector<int> segment_id;
    };

    template <typename T>
    class MeshExplicit
    {
    public:
        std::vector<Vertex<T>>       vertices;
        std::vector<Triangle>        triangles;
        std::vector<VertexAdjacency> adjacency;
        MeshAttributes<T>            attributes;

    public:
        MeshExplicit() = default;

        void build_adjacency()
        {
            adjacency.clear();
            adjacency.resize(vertices.size());

            for (std::uint32_t f = 0; f < triangles.size(); ++f)
            {
                const auto& tri = triangles[f];
                std::uint32_t vs[3] = { tri.v0, tri.v1, tri.v2 };

                for (int i = 0; i < 3; ++i)
                {
                    auto v = vs[i];
                    auto vn = vs[(i + 1) % 3];

                    adjacency[v].incident_faces.push_back(f);
                    adjacency[v].neighbor_vertices.push_back(vn);
                }
            }

            // Optional: remove duplicates from neighbor lists
            // INSERT: deduplication logic if desired
        }

        void reserve(std::size_t vertex_count, std::size_t triangle_count)
        {
            vertices.reserve(vertex_count);
            triangles.reserve(triangle_count);
            adjacency.reserve(vertex_count);
        }

        // some functions to add vertices and triangles are added here for convenience
        // although all the data members are public, we should still provide some helper functions 
        // to maintain consistency and avoid errors when adding data to the mesh.
        // For example, when adding a triangle, we should ensure that the vertex indices are valid 
        // and that the adjacency information is updated accordingly.
        std::uint32_t add_vertex(const basevector<T, 3>& position)
        {
            vertices.push_back({ position });
            return vertices.size() - 1; // return the index of the newly added vertex
        }

        std::uint32_t add_triangle(const Triangle& tri)
        {
            triangles.push_back(tri);
            return triangles.size() - 1; // return the index of the newly added triangle
        }

        std::uint32_t add_triangle(std::uint32_t v0, std::uint32_t v1, std::uint32_t v2)
        {
            return add_triangle({ v0, v1, v2 });
        }

        std::uint32_t triangle_count() const {
            return triangles.size();
        }

        const Triangle& get_triangle(std::uint32_t index) const {
            return triangles[index];
        }

        const Vertex<T>& get_vertex(std::uint32_t index) const {
            return vertices[index];
        }

    };

} // namespace btm_framework



#endif // __mesh_explicit_h__
