#ifndef __mesh_explicit_h__
#define __mesh_explicit_h__

#undef min
#undef max

#include "vector.h"
#include "triangle.h"
#include "aabb.h"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <queue>
#include <iterator>
#include <stdexcept>

namespace btm {
    template <typename T>
    struct VertexExplicit {
        basepoint3<T> position;
    };

    struct FaceExplicit {
        std::uint32_t v0, v1, v2;
        std::uint32_t v(int index) const {
            switch (index) {
                case 0: return v0;
                case 1: return v1;
                case 2: return v2;
                default: throw std::out_of_range("FaceExplicit::v index out of range");
            }
        }

        //bool flipped = false;
        bool visited = false;
        void flip() {
            std::swap(v1, v2);
        }
    };

    struct VertexAdjacency {
        std::vector<std::uint32_t> incident_faces;
        std::vector<std::uint32_t> neighbor_vertices;
    };

    struct EdgeAdjacency {
        std::vector<std::uint32_t> incident_faces;
    };

    struct FaceAdjacency {
        std::vector<std::uint32_t> neighbor_faces;
    };

    struct EdgeKey {
        std::uint32_t v0, v1;
        EdgeKey(std::uint32_t _v0, std::uint32_t _v1) : v0(std::min(_v0, _v1)), v1(std::max(_v0, _v1)) {}
        bool operator==(const EdgeKey& other) const noexcept {
            return v0 == other.v0 && v1 == other.v1;
        }
    };

    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    struct EdgeKeyHash {
        std::size_t operator()(const EdgeKey& e) const noexcept {
            std::size_t seed = 0;
            hash_combine(seed, e.v0);
            hash_combine(seed, e.v1);
            return seed;
        }
    };

    struct EdgeExplicit {
        EdgeKey desc;
        EdgeExplicit(std::uint32_t v0 = 0, std::uint32_t v1 = 0) : desc(v0, v1) {}
        std::vector<std::uint32_t> incident_faces;
    };

    template <typename T> struct MeshAttributes {
        std::vector<basevec3<T>> vertex_normals;
        std::vector<basevec3<T>> face_normals;

        // Curvature fields
        std::vector<T> k1; // principal curvature 1
        std::vector<T> k2; // principal curvature 2

        // Segmentation labels
        std::vector<int> segment_id;
    };

    template <typename T>
    class MeshExplicit {
    public:
        std::vector<VertexExplicit<T>> vertices;
        std::vector<FaceExplicit> faces;
        std::vector<VertexAdjacency> adjacency;
        std::vector<FaceAdjacency> face_adjacency;
        std::unordered_map<EdgeKey, EdgeExplicit, EdgeKeyHash> edges;
        MeshAttributes<T> attributes;

    public:
        MeshExplicit() = default;

        basevec3<T> vertex_position(std::uint32_t index) const {
            return vertices[index].position;
        }

        EdgeExplicit* find_edge(const EdgeKey& key) {
            auto it = edges.find(key);
            if (it == edges.end()) {
                // If the edge does not exist, create it
                return nullptr;
            }
            return &(it->second);
        }

        void adjacent_faces(std::uint32_t face, std::vector<std::uint32_t>& out_faces) const {
            EdgeKey e0(faces[face].v0, faces[face].v1);
            EdgeKey e1(faces[face].v1, faces[face].v2);
            EdgeKey e2(faces[face].v2, faces[face].v0);

            std::set<std::uint32_t> adj_faces;

            auto& fc1 = edges.find(e0)->second.incident_faces; // adjacent faces across edge e0
            auto& fc2 = edges.find(e1)->second.incident_faces; // adjacent faces across edge e1
            auto& fc3 = edges.find(e2)->second.incident_faces; // adjacent faces across edge e2
            adj_faces.insert(fc1.begin(), fc1.end());
            adj_faces.insert(fc2.begin(), fc2.end());
            adj_faces.insert(fc3.begin(), fc3.end());

            auto excludeCondition = [face](std::uint32_t value) {
                return value != face; // keep only odd numbers
                };

            std::copy_if(adj_faces.begin(), adj_faces.end(),
                std::back_inserter(out_faces),
                excludeCondition);
        }

        void recalculateMesh() {}
        AABB<T> getBoundingBox() {
            if (vertices.size() == 0) {
                return AABB<T>(); // empty bounding box
            }
            T min_x = std::numeric_limits<T>::max();
            T min_y = std::numeric_limits<T>::max();
            T min_z = std::numeric_limits<T>::max();
            T max_x = std::numeric_limits<T>::lowest();
            T max_y = std::numeric_limits<T>::lowest();
            T max_z = std::numeric_limits<T>::lowest();

            for (const auto& v_pair : vertices) {
                const basevec3<T>& coords = v_pair.position;
                if (coords.x() < min_x)
                    min_x = coords.x();
                if (coords.y() < min_y)
                    min_y = coords.y();
                if (coords.z() < min_z)
                    min_z = coords.z();
                if (coords.x() > max_x)
                    max_x = coords.x();
                if (coords.y() > max_y)
                    max_y = coords.y();
                if (coords.z() > max_z)
                    max_z = coords.z();
            }
            AABB<T> bbox({ min_x, min_y, min_z }, { max_x, max_y, max_z });
            return bbox;
        }
        void translate(const basevec3<T>& offset) {
            for (auto& v_pair : vertices) {
                basevec3<T>& coords = v_pair.position;
                coords = coords + offset;
            }
        }

        void flip_all_faces() {
            for (auto& face : faces) {
                face.flip();
            }
        }

        void build_adjacency() {
            adjacency.clear();
            adjacency.resize(vertices.size());

            face_adjacency.clear();
            face_adjacency.resize(faces.size());

            for (std::uint32_t f = 0; f < faces.size(); ++f) {
                const auto& face = faces[f];
                std::uint32_t vs[3] = { face.v0, face.v1, face.v2 };

                for (int i = 0; i < 3; ++i) {
                    auto v = vs[i];
                    auto vn = vs[(i + 1) % 3];

                    adjacency[v].incident_faces.push_back(f);
                    adjacency[v].neighbor_vertices.push_back(vn);
                }

                adjacent_faces(f, face_adjacency[f].neighbor_faces);
            }
            // Optional: remove duplicates from neighbor lists
            // INSERT: deduplication logic if desired
        }

        void build_attributes() {
            attributes.face_normals.clear();
            for (auto& face : faces) {
                const auto& v0 = vertices[face.v0].position;
                const auto& v1 = vertices[face.v1].position;
                const auto& v2 = vertices[face.v2].position;
                basevec3<T> edge1 = v1 - v0;
                basevec3<T> edge2 = v2 - v0;
                basevec3<T> face_normal = edge1.cross(edge2).normalize();
                attributes.face_normals.push_back(face_normal);
            }
        }

        void reserve(std::size_t vertex_count, std::size_t face_count) {
            vertices.reserve(vertex_count);
            faces.reserve(face_count);
            adjacency.reserve(vertex_count);
            face_adjacency.reserve(face_count);
        }

        // some functions to add vertices and faces are added here for
        // convenience although all the data members are public, we should still
        // provide some helper functions to maintain consistency and avoid errors
        // when adding data to the mesh. For example, when adding a face, we
        // should ensure that the vertex indices are valid and that the adjacency
        // information is updated accordingly.
        std::uint32_t add_vertex(size_t id, const basevec3<T>& position) {
            vertices.push_back({ position });
            return vertices.size() -
                1; // return the index of the newly added vertex
        }

        std::uint32_t add_face(const FaceExplicit& face) {
            faces.push_back(face);
            // Update edges
            std::uint32_t vs[3] = { face.v0, face.v1, face.v2 };
            for (int i = 0; i < 3; ++i) {
                std::uint32_t v0 = vs[i];
                std::uint32_t v1 = vs[(i + 1) % 3];
                EdgeKey desc(v0, v1);
                auto it = edges.find(desc);
                if (it == edges.end()) {
                    edges[desc] = EdgeExplicit(v0, v1);
                }
                edges[desc].incident_faces.push_back(faces.size() - 1);
            }
            return faces.size() - 1; // return the index of the newly added face
        }

        std::uint32_t add_face(std::uint32_t v0, std::uint32_t v1, std::uint32_t v2, std::uint32_t base) {
            return add_face({ v0 - base, v1 - base, v2 - base });
        }

        std::uint32_t add_face(std::uint32_t v0, std::uint32_t v1, std::uint32_t v2, std::uint32_t v3, std::uint32_t base) {
            // For quads, we can split them into two triangles (v0, v1, v2) and (v0, v2, v3)
            std::uint32_t f1 = add_face({ v0-base, v1-base, v2-base });
            std::uint32_t f2 = add_face({ v0-base, v2-base, v3-base });
            return f1; // return the index of the first triangle face
        }

        std::uint32_t face_count() const { return faces.size(); }

        const FaceExplicit& get_face(std::uint32_t index) const {
            return faces[index];
        }

        const VertexExplicit<T>& get_vertex(std::uint32_t index) const {
            return vertices[index];
        }

        void clear_faces_status() {
            for (auto& face : faces) {
                face.visited = false;
            }
        }

        void orient()
        {
            int face_count = faces.size();
            if (face_count == 0) return; // empty mesh, nothing to orient
            clear_faces_status();

            int seed = 0; // start from the first face
            faces[0].visited = true;
            std::queue<int> Q;
            Q.push(seed);
            int count = 1;
            while (!Q.empty()) {
                int f = Q.front(); Q.pop();

                FaceExplicit& face = faces[f];
                int nvF = 3; // Since it's a triangle

                for (int e = 0; e < nvF; e++) {
                    int a = face.v(e);
                    int b = face.v((e + 1) % nvF);

                    EdgeKey key(a, b);
                    auto* edge = find_edge(key);
                    if (!edge) continue; // edge not found, skip

                    for (auto& face_id : edge->incident_faces) {
                        if (face_id == f) continue; // skip the current face
                        FaceExplicit& adj_face = faces[face_id];
                        if (adj_face.visited) continue; // already visited
                        int nvG = 3; // Since it's a triangle

                        bool same_dir = false;
                        for (int k = 0; k < nvG; k++) {
                            int c = adj_face.v(k);
                            int d = adj_face.v((k + 1) % nvG);
                            if (c == a && d == b) {
                                same_dir = true;
                                break;
                            }
                        }
                        if (same_dir)
                        {
                            adj_face.flip(); // flip the adjacent face to maintain consistent orientation
                        }

                        adj_face.visited = true;
                        Q.push(face_id);
                        ++count;
                    }
                }
            }
        }


    };

    template <typename T>
    T component_signed_volume(btm::MeshExplicit<T>& mesh) {
        T vol = 0.0;
        for (FaceExplicit& face : mesh.faces) {
            int nv = 3; // triangles only

            auto tri_vol = [&](int i0, int i1, int i2) {
                btm::basevec3<T> a = mesh.vertex_position(i0);
                btm::basevec3<T> b = mesh.vertex_position(i1);
                btm::basevec3<T> c = mesh.vertex_position(i2);

                return btm::signedVolume(btm::Triangle<T>{a, b, c});
                };

            vol += tri_vol(face.v0, face.v1, face.v2);
        }

        return vol;
    }


} // namespace btm

#endif // __mesh_explicit_h__
