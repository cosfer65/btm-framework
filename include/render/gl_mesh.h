#pragma once

#include <vector>
#include "vector.h"

#include "mesh_explicit.h"

using namespace btm;

namespace btm {
    struct mesh_data {
        size_t num_vertices = 0;                ///< Number of vertices in the mesh.
        size_t num_normals = 0;                 ///< Number of normals in the mesh.
        size_t num_indices = 0;                 ///< Number of indices in the mesh.
        size_t num_curvatures = 0;              ///< Number of curvature values (if available).
        std::vector<float> vertices;            ///< Flat array of meshVertex positions (x, y, z).
        std::vector<float> normals;             ///< Flat array of normal vectors (x, y, z).
        std::vector<unsigned int> indices;      ///< Indices defining mesh faces.
        std::vector<float> curvatures;          ///< Optional array of curvature values per meshVertex (if available). [per vertex color support]

        int add_vertex(const fvec3& v) {
            vertices.push_back(v.x());
            vertices.push_back(v.y());
            vertices.push_back(v.z());
            // divide to get to the number of vertices, not the number of floats
            num_vertices = vertices.size()/3;
            return (int)(num_vertices - 1);
        }
        int add_indices(unsigned int i1, unsigned int i2, unsigned int i3) {
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
            num_indices = indices.size();
            return (int)(num_indices - 1);
        }
        int add_indices(unsigned int i1, unsigned int i2) {
            indices.push_back(i1);
            indices.push_back(i2);
            num_indices = indices.size();
            return (int)(num_indices - 1);
        }
        int add_index(unsigned int i1) {
            indices.push_back(i1);
            num_indices = indices.size();
            return (int)(num_indices - 1);
        }
        int add_normal(const fvec3& n) {
            normals.push_back(n.x());
            normals.push_back(n.y());
            normals.push_back(n.z());
            num_normals = normals.size() / 3;
            return (int)(num_normals - 1);
        }
        int add_curvature(const fvec3& c) {
            curvatures.push_back(c.x());
            curvatures.push_back(c.y());
            curvatures.push_back(c.z());
            num_curvatures = curvatures.size() / 3;
            return (int)(num_curvatures - 1);
        }
        int add_color(const fvec3& c) {
            return add_curvature(c);
        }
    };

    template <typename T>
    T mix(T a, T b, float t) {
        return a * (1.0f - t) + b * t;
    }

    template <typename T, typename U>
    T mix(T a, T b, T c, U t)
    {
        U w1 = (U(1.0) - t) * (U(1.0) - t);
        U w2 = U(2.0) * t * (U(1.0) - t);
        U w3 = t * t;
        return a * w1 + b * w2 + c * w3;
    }


    template <typename T>
    void collect_mesh_data(const MeshExplicit<T>* mesh, mesh_data& mdata) {
        static fvec3 red = fvec3(1, 0, 0);
        static fvec3 blue = fvec3(0, 0, 1);
        static fvec3 green = fvec3(0, 1, 0);
        static fvec3 yellow = fvec3(1, 1, 0);
        static fvec3 magenta = fvec3(1, 0, 1);
        static fvec3 cyan = fvec3(0, 1, 1);
        static fvec3 white = fvec3(1, 1, 1);
        static fvec3 black = fvec3(0, 0, 0);
        static fvec3 gray = fvec3(0.5f, 0.5f, 0.5f);
        static std::vector<fvec3> colors = { red, blue, green, yellow, magenta, cyan, white, black, gray };

        bool curvatures_calculated = mesh->curvatures_calculated();
        // This function converts the MeshExplicit data into a flat format suitable for OpenGL rendering.
        // It iterates over the triangles in the mesh and extracts vertex positions to fill the mesh_data structure.
        size_t index = 0;
        // do NOT call size() on the vector all the time, as functioncalls are expensive
        // instead, store the size in a variable and use that for the loop condition
        size_t num_faces = mesh->faces.size();
        for (size_t cur_face = 0; cur_face < num_faces; ++cur_face) {
            const auto& face = mesh->faces[cur_face];
            const auto& v0 = mesh->vertices[face.v0].position;
            const auto& v1 = mesh->vertices[face.v1].position;
            const auto& v2 = mesh->vertices[face.v2].position;

            const auto& norm = face.normal;

            mdata.vertices.push_back(static_cast<float>(v0.x()));
            mdata.vertices.push_back(static_cast<float>(v0.y()));
            mdata.vertices.push_back(static_cast<float>(v0.z()));

            mdata.normals.push_back(static_cast<float>(norm.x()));
            mdata.normals.push_back(static_cast<float>(norm.y()));
            mdata.normals.push_back(static_cast<float>(norm.z()));

            if (curvatures_calculated) {
                const auto& c0 = mesh->vertex_curvatures[face.v0];
                mdata.add_curvature(colors[c0.curvature_map_value]);
            }

            mdata.indices.push_back(static_cast<unsigned int>(index));
            ++index;

            mdata.vertices.push_back(static_cast<float>(v1.x()));
            mdata.vertices.push_back(static_cast<float>(v1.y()));
            mdata.vertices.push_back(static_cast<float>(v1.z()));

            mdata.normals.push_back(static_cast<float>(norm.x()));
            mdata.normals.push_back(static_cast<float>(norm.y()));
            mdata.normals.push_back(static_cast<float>(norm.z()));

            if (curvatures_calculated) {
                const auto& c1 = mesh->vertex_curvatures[face.v1];
                mdata.add_curvature(colors[c1.curvature_map_value]);
            }

            mdata.indices.push_back(static_cast<unsigned int>(index));
            ++index;

            mdata.vertices.push_back(static_cast<float>(v2.x()));
            mdata.vertices.push_back(static_cast<float>(v2.y()));
            mdata.vertices.push_back(static_cast<float>(v2.z()));

            mdata.normals.push_back(static_cast<float>(norm.x()));
            mdata.normals.push_back(static_cast<float>(norm.y()));
            mdata.normals.push_back(static_cast<float>(norm.z()));

            if (curvatures_calculated) {
                const auto& c2 = mesh->vertex_curvatures[face.v2];
                mdata.add_curvature(colors[c2.curvature_map_value]);
            }

            mdata.indices.push_back(static_cast<unsigned int>(index));
            ++index;
        }
        mdata.num_vertices = mdata.vertices.size()/3;
        mdata.num_normals = mdata.normals.size()/3;
        mdata.num_indices = mdata.indices.size();
        mdata.num_curvatures = mdata.curvatures.size()/3;
    }
}
