#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace btm {
    struct HalfEdgeMesh {
        using index_t = std::uint32_t;
        static constexpr index_t invalid = std::numeric_limits<index_t>::max();

        struct Vertex {
            index_t halfedge = invalid;   // one outgoing half-edge (origin at this vertex)
        };

        struct Face {
            index_t halfedge = invalid;   // one half-edge on the face cycle
        };

        struct HalfEdge {
            index_t target = invalid;     // vertex this half-edge points TO
            index_t origin = invalid;     // vertex this half-edge originates FROM
            index_t twin = invalid;     // opposite half-edge
            index_t next = invalid;     // next half-edge in face cycle
            index_t face = invalid;     // face this half-edge belongs to
        };

        std::vector<Vertex>   vertices;
        std::vector<Face>     faces;
        std::vector<HalfEdge> halfedges;

        void clear() {
            vertices.clear();
            faces.clear();
            halfedges.clear();
        }

        index_t add_vertex() {
            vertices.emplace_back();
            return static_cast<index_t>(vertices.size() - 1);
        }

        index_t add_face() {
            faces.emplace_back();
            return static_cast<index_t>(faces.size() - 1);
        }

        index_t add_halfedge(index_t origin, index_t target) {
            halfedges.emplace_back();
            halfedges.back().origin = origin;
            halfedges.back().target = target;
            return static_cast<index_t>(halfedges.size() - 1);
        }

        bool is_valid(index_t h) const {
            return h != invalid && h < halfedges.size();
        }

        bool is_boundary(index_t h) const {
            return !is_valid(halfedges[h].face);
        }

        index_t twin(index_t h) const { return halfedges[h].twin; }
        index_t next(index_t h) const { return halfedges[h].next; }
        index_t to_vertex(index_t h) const { return halfedges[h].target; }
        index_t face_of(index_t h) const { return halfedges[h].face; }

        // ------------------------------------------------------------
        // Face cycle iteration
        // ------------------------------------------------------------
        template <typename Func>
        void for_each_halfedge_in_face(index_t face_id, Func func) const {
            index_t h0 = faces[face_id].halfedge;
            index_t h = h0;
            do {
                func(h);
                h = halfedges[h].next;
            } while (h != h0);
        }

        // ------------------------------------------------------------
        // Vertex outgoing half-edges (around vertex)
        // ------------------------------------------------------------
        template <typename Func>
        void for_each_outgoing_halfedge(index_t v, Func func) const {
            index_t h0 = vertices[v].halfedge;
            if (!is_valid(h0)) return;

            index_t h = h0;
            do {
                func(h);
                index_t ht = halfedges[h].twin;
                if (!is_valid(ht)) break; // safety on broken topology
                h = halfedges[ht].next;
            } while (h != h0 && is_valid(h));
        }

        // ------------------------------------------------------------
        // Vertex neighbors (1-ring)
        // ------------------------------------------------------------
        template <typename Func>
        void for_each_vertex_neighbor(index_t v, Func func) const {
            index_t h0 = vertices[v].halfedge;
            if (!is_valid(h0)) return;

            index_t h = h0;
            do {
                func(halfedges[h].target); // TO vertex of outgoing half-edge
                index_t ht = halfedges[h].twin;
                if (!is_valid(ht)) break;
                h = halfedges[ht].next;
            } while (h != h0 && is_valid(h));
        }

        // ------------------------------------------------------------
        // Faces incident to vertex
        // ------------------------------------------------------------
        template <typename Func>
        void for_each_face_incident(index_t v, Func func) const {
            index_t h0 = vertices[v].halfedge;
            if (!is_valid(h0)) return;

            index_t h = h0;
            do {
                index_t f = halfedges[h].face;
                if (f != invalid)
                    func(f);

                index_t ht = halfedges[h].twin;
                if (!is_valid(ht)) break;
                h = halfedges[ht].next;
            } while (h != h0 && is_valid(h));
        }

        // ------------------------------------------------------------
        // Iterate unique edges
        // ------------------------------------------------------------
        template <typename Func>
        void for_each_edge(Func func) const {
            for (index_t h = 0; h < halfedges.size(); ++h) {
                index_t t = halfedges[h].twin;
                func(h, t);
            }
        }

        // ------------------------------------------------------------
        // Edge helpers
        // ------------------------------------------------------------
        index_t edge_to(index_t h) const {
            return halfedges[h].target;
        }

        index_t edge_from(index_t h) const {
            index_t prev = invalid;
            // prev in face cycle: find half-edge whose next == h
            for (index_t i = 0; i < halfedges.size(); ++i) {
                if (halfedges[i].next == h) {
                    prev = i;
                    break;
                }
            }
            return is_valid(prev) ? halfedges[prev].target : invalid;
        }

        index_t edge_left_face(index_t h) const {
            return halfedges[h].face;
        }

        index_t edge_right_face(index_t h) const {
            index_t ht = halfedges[h].twin;
            return is_valid(ht) ? halfedges[ht].face : invalid;
        }

        // ------------------------------------------------------------
        // Build half-edge mesh from explicit mesh
        // ------------------------------------------------------------
        template <typename MESH>
        void build_from_explicit(const MESH& mesh) {
            clear();

            auto  tris = mesh.get_faces();

            vertices.resize(mesh.num_vertices());
            faces.resize(mesh.num_faces());
            halfedges.reserve(mesh.num_faces() * 3);

            struct EdgeKey {
                index_t a, b;
                bool operator==(const EdgeKey& o) const { return a == o.a && b == o.b; }
            };

            struct EdgeKeyHash {
                std::size_t operator()(const EdgeKey& k) const {
                    return (std::size_t(k.a) << 32) ^ std::size_t(k.b);
                }
            };

            std::unordered_map<EdgeKey, index_t, EdgeKeyHash> edge_map;

            for (index_t f = 0; f < tris.size(); ++f) {
                const auto& tri = tris[f];

                index_t v0 = tri.v0;
                index_t v1 = tri.v1;
                index_t v2 = tri.v2;

                index_t h0 = add_halfedge(v0, v1); // v0 -> v1
                index_t h1 = add_halfedge(v1, v2); // v1 -> v2
                index_t h2 = add_halfedge(v2, v0); // v2 -> v0

                faces[f].halfedge = h0;

                halfedges[h0].face = f;
                halfedges[h1].face = f;
                halfedges[h2].face = f;

                halfedges[h0].next = h1;
                halfedges[h1].next = h2;
                halfedges[h2].next = h0;

                auto register_edge = [&](index_t from, index_t to, index_t h) {
                    EdgeKey key{ std::min(from, to), std::max(from, to) };

                    auto it = edge_map.find(key);
                    if (it == edge_map.end()) {
                        edge_map[key] = h;
                    }
                    else {
                        index_t h_twin = it->second;
                        halfedges[h].twin = h_twin;
                        halfedges[h_twin].twin = h;
                    }
                    };

                register_edge(v0, v1, h0);
                register_edge(v1, v2, h1);
                register_edge(v2, v0, h2);

                // IMPORTANT: assign outgoing half-edge from ORIGIN vertex
                if (vertices[v0].halfedge == invalid) vertices[v0].halfedge = h0;
                if (vertices[v1].halfedge == invalid) vertices[v1].halfedge = h1;
                if (vertices[v2].halfedge == invalid) vertices[v2].halfedge = h2;
            }
        }

        // ------------------------------------------------------------
        // Build half-edge mesh from triangle list
        // ------------------------------------------------------------
        void build(int num_vertices, const std::vector<std::tuple<int, int, int>>& tris) {
            clear();

            vertices.resize(num_vertices);
            faces.resize(tris.size());
            halfedges.reserve(tris.size() * 3);

            struct EdgeKey {
                index_t a, b;
                bool operator==(const EdgeKey& o) const { return a == o.a && b == o.b; }
            };

            struct EdgeKeyHash {
                std::size_t operator()(const EdgeKey& k) const {
                    return (std::size_t(k.a) << 32) ^ std::size_t(k.b);
                }
            };

            std::unordered_map<EdgeKey, index_t, EdgeKeyHash> edge_map;

            for (index_t f = 0; f < tris.size(); ++f) {
                const auto& tri = tris[f];

                index_t v0 = std::get<0>(tri);
                index_t v1 = std::get<1>(tri);
                index_t v2 = std::get<2>(tri);

                index_t h0 = add_halfedge(v0, v1); // v0 -> v1
                index_t h1 = add_halfedge(v1, v2); // v1 -> v2
                index_t h2 = add_halfedge(v2, v0); // v2 -> v0

                faces[f].halfedge = h0;

                halfedges[h0].face = f;
                halfedges[h1].face = f;
                halfedges[h2].face = f;

                halfedges[h0].next = h1;
                halfedges[h1].next = h2;
                halfedges[h2].next = h0;

                auto register_edge = [&](index_t from, index_t to, index_t h) {
                    EdgeKey key{ std::min(from, to), std::max(from, to) };

                    auto it = edge_map.find(key);
                    if (it == edge_map.end()) {
                        edge_map[key] = h;
                    }
                    else {
                        index_t h_twin = it->second;
                        halfedges[h].twin = h_twin;
                        halfedges[h_twin].twin = h;
                    }
                    };

                register_edge(v0, v1, h0);
                register_edge(v1, v2, h1);
                register_edge(v2, v0, h2);

                // IMPORTANT: assign outgoing half-edge from ORIGIN vertex
                if (vertices[v0].halfedge == invalid) vertices[v0].halfedge = h0;
                if (vertices[v1].halfedge == invalid) vertices[v1].halfedge = h1;
                if (vertices[v2].halfedge == invalid) vertices[v2].halfedge = h2;
            }
        }

        // ------------------------------------------------------------
        // Create boundary half-edges for edges without twins
        // ------------------------------------------------------------
        void build_boundary_halfedges() {
            const index_t H = static_cast<index_t>(halfedges.size());

            for (index_t h = 0; h < H; ++h) {
                if (halfedges[h].twin != invalid)
                    continue;

                index_t v_to = halfedges[h].origin;
                index_t v_from = halfedges[h].target;

                index_t hb = add_halfedge(v_from, v_to);

                halfedges[h].twin = hb;
                halfedges[hb].twin = h;

                halfedges[hb].face = invalid; // boundary
                halfedges[hb].next = invalid; // will be set in boundary cycles
            }
        }

        // ------------------------------------------------------------
        // Robust boundary cycle builder
        // ------------------------------------------------------------
        void build_boundary_cycles() {
            const index_t H = static_cast<index_t>(halfedges.size());
            std::vector <index_t> boundary_halfedges;

            // collect all boundary half-edges
            for (index_t h = 0; h < H; ++h) {
                if (halfedges[h].face != invalid)
                    continue; // not a boundary half-edge
                boundary_halfedges.push_back(h);
            }

            // build chains using boundary half-edges

            // build a fast lookup from origin -> boundary half-edge
            std::unordered_map<index_t, index_t> origin_to_he;
            origin_to_he.reserve(boundary_halfedges.size() * 2 + 1);
            for (index_t h : boundary_halfedges) {
                origin_to_he[halfedges[h].origin] = h;
            }

            // track which half-edges we've linked to avoid revisiting
            std::unordered_set<index_t> linked;
            linked.reserve(boundary_halfedges.size() * 2 + 1);

            for (index_t start_he : boundary_halfedges) {
                if (linked.find(start_he) != linked.end())
                    continue;

                index_t start = start_he;
                index_t end = start_he;

                while (true) {
                    auto it = origin_to_he.find(halfedges[end].target);
                    if (it == origin_to_he.end())
                        break; // no matching boundary half-edge
                    index_t hB = it->second;
                    if (halfedges[end].next != invalid)
                        break; // already linked
                    halfedges[end].next = hB;
                    // std::cout << "linking boundary half-edge " << end << " to " << hB << "\n";
                    linked.insert(end);
                    end = hB;
                    if (end == start)
                    {
                        linked.insert(end);
                        break; // closed cycle
                    }
                    if (linked.find(end) != linked.end())
                        break; // reached already processed half-edge, stop to avoid loops
                }
            }
        }

        // ------------------------------------------------------------
        // Validation
        // ------------------------------------------------------------
        bool validate() const {
            for (index_t h = 0; h < halfedges.size(); ++h) {
                const auto& HE = halfedges[h];

                if (HE.target >= vertices.size()) return false;
                if (HE.twin != invalid && HE.twin >= halfedges.size()) return false;
                if (HE.next != invalid && HE.next >= halfedges.size()) return false;
                if (HE.face != invalid && HE.face >= faces.size()) return false;

                if (HE.twin != invalid)
                    if (halfedges[HE.twin].twin != h)
                        return false;
            }
            return true;
        }
    };
} // namespace btm
