#ifndef __mesh_explicit_h__
#define __mesh_explicit_h__

#undef min
#undef max

#include "vector.h"
#include "triangle.h"
#include "aabb.h"
#include "geometry.h"
#include "btm_vector.h"
#include "hash.h"

#include <map>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <string>
#include <algorithm>

namespace btm {
    template <typename T>
    struct VertexExplicit {
        btm::basepoint3<T> position;
        btm::basevec3<T> normal; // Normal at the vertex
        T voronoi_area; // Voronoi area associated with the vertex
        T area_sum; // Sum of areas of incident faces for the vertex
        T angle_sum; // Sum of corner angles at the vertex
        bool is_boundary; // Flag indicating if the vertex is on the boundary
    };

    template <typename T>
    struct FaceExplicit {
        std::uint32_t v0, v1, v2;
        btm::basevec3<T> normal; // Normal of the face
        btm::basepoint3<T> center; // Center of the face
        T angles[3]; // corner angles at v0, v1, v2
        T area; // Area of the face
        bool visited = false;

        std::uint32_t v(int index) const {
            switch (index) {
            case 0: return v0;
            case 1: return v1;
            case 2: return v2;
            default: throw std::out_of_range("FaceExplicit::v index out of range");
            }
        }

        T angle(int vertex_id) const {
            if (vertex_id == v0) return angles[0];
            if (vertex_id == v1) return angles[1];
            if (vertex_id == v2) return angles[2];
            throw std::out_of_range("FaceExplicit::angle vertex_id out of range");
        }

        void flip() {
            std::swap(v1, v2);
            std::swap(angles[1], angles[2]);
            normal = -normal;
        }
        void translate(const btm::basevec3<T>& translation) {
            center += translation;
        }
    };

    struct VertexAdjacency {
        btm::btm_vector<std::uint32_t> incident_faces;     // Faces that share this vertex
        btm::btm_vector<std::uint32_t> neighbor_vertices;  // Vertices that are neighbors of this vertex
    };

    struct FaceAdjacency {
        std::uint32_t neighbor_faces[3];  // Faces that share an edge with this face
        int neighbor_faces_size = 0;      // Number of neighbor faces currently stored
        void clear() {
            neighbor_faces_size = 0;
        }
        void add_neighbor_face(std::uint32_t face_id) {
            if (neighbor_faces_size < 3) {
                neighbor_faces[neighbor_faces_size++] = face_id;
            }
        }
    };

    using edge_key = std::uint64_t;

    struct EdgeExplicit {
        static edge_key make_edge_key(std::uint32_t v0, std::uint32_t v1) {
            if (v0 > v1) std::swap(v0, v1);
            return (static_cast<edge_key>(v0) << 32) | static_cast<edge_key>(v1);
        }
        edge_key key;  // Unique key for the edge
        EdgeExplicit(std::uint32_t v0 = 0, std::uint32_t v1 = 0) : key(EdgeExplicit::make_edge_key(v0, v1)) {}
        std::uint32_t v0() const {
            return key >> 32; // desc.v0;
        }
        std::uint32_t v1() const {
            return key & 0xFFFFFFFF; // desc.v1;
        }

        // each edge can be incident to at most two faces in a triangle mesh, so we can store the incident face indices in a fixed-size array
        int incident_faces[2] = { -1, -1 };       // Store up to two incident faces for a triangle edge
        int incident_faces_size = 0;              // Number of incident faces currently stored
        int f0() const {
            return incident_faces[0];
        }
        int f1() const {
            return incident_faces[1];
        }
        inline bool is_boundary() const {         // An edge is a boundary edge if it is incident to only one face
            return incident_faces_size < 2;
        }
        inline void add_incident_face(std::uint32_t face_id) {
            if (face_id < 0) return; // invalid face id
            if (incident_faces_size == 0) {
                incident_faces[incident_faces_size++] = face_id;
            } else if (incident_faces_size == 1) {
                if (incident_faces[0] != face_id)
                    incident_faces[incident_faces_size++] = face_id;
            }

            /*if (incident_faces_size < 2) {
                incident_faces[incident_faces_size++] = face_id;
            }*/
        }
    };

    template <typename T>
    struct VertexCurvature {
        T kmin;         // principal min
        T kmax;         // principal max;

        T mean;       // Mean curvature H = (k_1 + k_2) / 2.
        T gaussian;   // Gaussian curvature K = k_1 * k_2.

        /// Principal directions corresponding to 'principal_curvatures'.
        btm::basevec3<T> kmin_dir;
        btm::basevec3<T> kmax_dir;

        btm::basevec3<T> meanCurvatureDir; // this can be computed as the normalized sum of the
        // principal curvature directions weighted by their
        // respective curvatures, i.e., (k_1 * k_1_dir + k_2 *
        // k_2_dir) / (k_1 + k_2), and it points in the
        // direction of the surface normal if the mean
        // curvature is positive (convex) and in the opposite
        // direction if the mean curvature is negative
        // (concave).

        // cached derived quantities for convenience
        /// Absolute value of the minimum principal curvature |k1|.
        T abs_kmin;
        /// Absolute value of the maximum principal curvature |k2|.
        T abs_kmax;
        /// Absolute value of mean curvature |H|.
        T absMeanCurvature;
        /// absolute value of Gaussian curvature.
        T absGaussCurvature;

        /// Sign of Gaussian curvature K (e.g., -1 for saddle, 0 for flat, 1 for
        /// elliptic).
        int signGauss;
        /// Sign of mean curvature H (e.g., -1 for concave, 0 for minimal, 1 for
        /// convex).
        int signMean;

        int curvature_map_value = -1; // for visualization purposes, e.g., 0 for negative curvature, 1 for flat, 2 for positive curvature

        /// @brief Default-initialize all curvature values to zero and label to
        /// unknown.
        VertexCurvature()
            : kmin(0), kmax(0), mean(0), gaussian(0), abs_kmin(0),
            abs_kmax(0), absMeanCurvature(0), absGaussCurvature(0), signGauss(0),
            signMean(0) {}
        void reset() {
            kmin = 0;
            kmax = 0;
            mean = 0;
            gaussian = 0;
            abs_kmin = 0;
            abs_kmax = 0;
            absMeanCurvature = 0;
            absGaussCurvature = 0;
            signGauss = 0;
            signMean = 0;
        }
    };

    template <typename T> using CurvatureField = btm::btm_vector<VertexCurvature<T>>;

    template <typename T>
    class MeshExplicit {
    public:
        btm::btm_vector<VertexExplicit<T>> vertices;
        CurvatureField<T> vertex_curvatures;
        btm::btm_vector<FaceExplicit<T>> faces;
        // edges are stored in a map for efficient lookup by edge key
        // the edge key is a 64-bit integer that encodes the two vertex indices of the edge
        // ensuring we have a unique representation for each edge, regardless of the order of the vertices
        std::map<edge_key, EdgeExplicit> edges;
        // accessing the edges sequentially is not efficient with a map, 
        // so we maintain a separate vector of edge keys for sequential access
        // this will also help us create a unified access pattern for edges, similar to vertices and faces
        // as well as serve as common interface for different types of meshes, e.g., explicit, half-edge, etc.
        std::vector<edge_key> edges_keys;

        // adjacency information for efficient traversal and neighborhood queries
        btm::btm_vector<VertexAdjacency> vertex_adjacency;     // faces incident to each vertex and neighboring vertices
        btm::btm_vector<FaceAdjacency> face_adjacency;  // faces adjacent to each face

        T m_average_edge_length = T(0); // average edge length of the mesh, useful for scaling display elements like normals, curvature vectors, etc.

    public:
        MeshExplicit() = default;

        size_t num_vertices() const { return vertices.size(); }
        size_t num_faces() const { return faces.size(); }
        size_t num_edges() const { return edges.size(); }

        bool curvatures_calculated() const {
            return vertex_curvatures.size() > 0 && vertex_curvatures.size() == vertices.size();
        }


        const btm::btm_vector<VertexExplicit<T>>& get_vertices() const { return vertices; }
        const btm::btm_vector<FaceExplicit<T>>& get_faces() const { return faces; }
        const btm::btm_vector<VertexCurvature<T>>& get_vertex_curvatures() const { return vertex_curvatures; }
        const std::unordered_map<edge_key, EdgeExplicit>& get_edges() const { return edges; }
        btm::basepoint3<T> get_vertex_position(std::uint32_t vertex_index) const { return vertices[vertex_index].position; }

        bool collect_edges_keys(std::vector<edge_key>& ev) {
            ev.clear();
            for (auto e : edges) {
                ev.push_back(e.first);
            }
            return true;
        }
        EdgeExplicit& edge(int index) {
            edge_key key = edges_keys[index];
            return edges[key];
        }
        const EdgeExplicit& edge(int index) const {
            edge_key key = edges_keys[index];
            return edges.at(key);
        }
        void generate_edges() {
            edges.clear();
            edges_keys.clear();
            std::uint32_t face_count = 0;
            size_t num_faces = faces.size();
            for (std::uint32_t f = 0; f < num_faces; ++f) {
                FaceExplicit<T>& face = faces[f];
                std::uint32_t vs[3] = { face.v0, face.v1, face.v2 };
                for (int i = 0; i < 3; ++i) {
                    std::uint32_t v0 = vs[i];
                    std::uint32_t v1 = vs[(i + 1) % 3];
                    edge_key desc = EdgeExplicit::make_edge_key(v0, v1);
                    auto it = edges.find(desc);
                    if (it == edges.end()) {
                        edges[desc] = EdgeExplicit(v0, v1);
                        edges_keys.push_back(desc);  // Store the edge key for sequential access
                    }
                    edges[desc].add_incident_face(f);
                }
            }
            // collect_edges_keys(edges_keys);
        }

        T compute_angle_sum(size_t vertex_index) {
            T angle_sum = T(0);
            VertexAdjacency& va = vertex_adjacency[vertex_index];
            for (size_t j = 0; j < va.incident_faces.size(); ++j) {
                FaceExplicit<T>& face = faces[va.incident_faces[j]];
                angle_sum += face.angle(vertex_index);
            }
            return angle_sum;
        }


        void calculate_vertex_normals() {
            // for all vertices
            size_t num_vertices = vertices.size();
            for (size_t i = 0; i < num_vertices; ++i) {
                VertexExplicit<T>& vertex = vertices[i];
                btm::basevec3<T> norm = btm::basevec3<T>(0, 0, 0);
                // for all incident faces of vertex i
                VertexAdjacency& va = vertex_adjacency[i];
                size_t incident_face_count = va.incident_faces.size();
                for (size_t j = 0; j < incident_face_count; ++j) {
                    FaceExplicit<T>& face = faces[va.incident_faces[j]];
                    norm += face.normal;
                }
                // Normalize the normal vector
                vertex.normal = norm.normalize();
            }
        }

        // Compute vertex normals based on face normals and corner angles
        // area_sums and angle_sums as well
        void compute_vertex_attributes() {
            // for all vertices
            size_t num_vertices = vertices.size();
            for (size_t i = 0; i < num_vertices; ++i) {
                VertexExplicit<T>& vertex = vertices[i];
                btm::basevec3<T> norm = btm::basevec3<T>(0, 0, 0);
                T sum_area = T(0);
                T sum_angle = T(0);
                // for all incident faces of vertex i
                VertexAdjacency& va = vertex_adjacency[i];
                size_t incident_face_count = va.incident_faces.size();
                for (size_t j = 0; j < incident_face_count; ++j) {
                    FaceExplicit<T>& face = faces[va.incident_faces[j]];
                    T a0 = face.angle(i);
                    norm += face.normal;
                    sum_area += face.area;
                    sum_angle += a0;
                }
                // set the vertex attributes
                // Normalize the normal vector
                vertex.normal = norm.normalize();
                vertex.area_sum = sum_area;
                vertex.angle_sum = sum_angle;
            }
        }

        void calculate_face_normals() {
            for (auto& face : faces) {
                const auto& v0 = vertices[face.v0].position;
                const auto& v1 = vertices[face.v1].position;
                const auto& v2 = vertices[face.v2].position;
                btm::basevec3<T> edge1 = v1 - v0;
                btm::basevec3<T> edge2 = v2 - v0;
                face.normal = edge1.cross(edge2).normalize();
            }
        }

        void calculate_face_attributes() {
            for (auto& face : faces) {
                const auto& v0 = vertices[face.v0].position;
                const auto& v1 = vertices[face.v1].position;
                const auto& v2 = vertices[face.v2].position;
                btm::basevec3<T> edge1 = v1 - v0;
                btm::basevec3<T> edge2 = v2 - v0;
                face.normal = edge1.cross(edge2).normalize();
                face.center = (v0 + v1 + v2) / T(3);
                face.area = edge1.cross(edge2).length() * T(0.5);
            }
        }

        void compute_voronoi_areas() {
            // for all vertices
            size_t num_vertices = vertices.size();
            for (size_t i = 0; i < num_vertices; ++i) {
                VertexExplicit<T>& vertex = vertices[i];
                T area_mixed = T(0);
                btm::basevec3<T>& P = vertex.position;
                // for each incident face of vertex i, compute the Voronoi area contribution
                VertexAdjacency& va = vertex_adjacency[i];
                for (size_t j = 0; j < va.incident_faces.size(); ++j) {
                    std::uint32_t face_id = va.incident_faces[j];
                    FaceExplicit<T>& face = faces[face_id];
                    int v1, v2;
                    opposite_vertices(face, i, v1, v2);
                    btm::basevec3<T> v1p = vertices[v1].position;
                    btm::basevec3<T> v2p = vertices[v2].position;
                    btm::basevec3<T> PR = v2p - P;
                    btm::basevec3<T> PQ = v1p - P;
                    btm::basevec3<T> QR = v2p - v1p;

                    // Total area of this triangle (calculating it is faster than using the precomputed face area due to cache locality)
                    T area_tri = T(0.5) * PQ.cross(PR).length();
                    // Check if triangle is obtuse
                    bool is_obtuse = PQ.dot(PR) < 0 || (-PQ).dot(QR) < 0 || (-PR).dot(-QR) < 0;
                    if (!is_obtuse) {
                        // Voronoi Area using cotangent weights
                        T cot_alpha = PQ.dot(PR) / PQ.cross(PR).length();
                        T cot_beta = (-PQ).dot(QR) / (-PQ).cross(QR).length();
                        area_mixed += (cot_alpha * PR.length() * PR.length() +
                            cot_beta * PQ.length() * PQ.length()) /
                            T(8.0);
                    }
                    else {
                        // Angle at Q or R is obtuse
                        area_mixed += area_tri / T(4.0);
                    }
                }
                // Store the computed Voronoi area in the vertex's voronoi area attribute
                vertex.voronoi_area = area_mixed;
            }
        }

        // Given a face and a vertex index, find the two other vertices of the face
        void opposite_vertices(const FaceExplicit<T>& face, std::uint32_t vertex_index, int& i1, int& i2) const {
            if (face.v0 == vertex_index) {
                i1 = face.v1;
                i2 = face.v2;
            }
            else if (face.v1 == vertex_index) {
                i1 = face.v2;
                i2 = face.v0;
            }
            else if (face.v2 == vertex_index) {
                i1 = face.v0;
                i2 = face.v1;
            }
            else {
                throw std::runtime_error("Vertex index not found in face");
            }
        }

        T average_edge_length() const {
            return m_average_edge_length;
        }

        EdgeExplicit* find_edge(const edge_key& key) {
            auto it = edges.find(key);
            if (it == edges.end()) {
                // If the edge does not exist, create it
                return nullptr;
            }
            return &(it->second);
        }

        inline EdgeExplicit* find_edge(std::uint32_t v0, std::uint32_t v1) {
            edge_key key = EdgeExplicit::make_edge_key(v0, v1);
            return find_edge(key);
        }

        inline void adjacent_faces(std::uint32_t face, FaceAdjacency& out_faces) const {
            FaceExplicit<T> const& f = faces[face];
            edge_key e0 = EdgeExplicit::make_edge_key(f.v0, f.v1);
            edge_key e1 = EdgeExplicit::make_edge_key(f.v1, f.v2);
            edge_key e2 = EdgeExplicit::make_edge_key(f.v2, f.v0);

            auto& fc1 = edges.find(e0)->second.incident_faces; // adjacent faces across edge e0
            auto& fc2 = edges.find(e1)->second.incident_faces; // adjacent faces across edge e1
            auto& fc3 = edges.find(e2)->second.incident_faces; // adjacent faces across edge e2

            out_faces.clear();
            if (fc1[0] != face) out_faces.add_neighbor_face(fc1[0]);
            else if (fc1[1] >= 0 && fc1[1] != face) out_faces.add_neighbor_face(fc1[1]);
            if (fc2[0] != face) out_faces.add_neighbor_face(fc2[0]);
            else if (fc2[1] >= 0 && fc2[1] != face) out_faces.add_neighbor_face(fc2[1]);
            if (fc3[0] != face) out_faces.add_neighbor_face(fc3[0]);
            else if (fc3[1] >= 0 && fc3[1] != face) out_faces.add_neighbor_face(fc3[1]);
        }
        // error C2662 : 'uint32_t btm::EdgeExplicit::v1(void)' : cannot convert 'this' pointer from 'const btm::EdgeExplicit' to 'btm::EdgeExplicit &'
        void recalculateMesh() {
            T total_edge_length = T(0);
            for (const auto& edge_pair : edges) {
                const EdgeExplicit& edge = edge_pair.second;
                int vv0 = edge.v0();
                int vv1 = edge.v1();
                const btm::basevec3<T>& v0 = vertices[vv0].position;
                const btm::basevec3<T>& v1 = vertices[vv1].position;
                total_edge_length += (v1 - v0).length();
            }
            m_average_edge_length = total_edge_length / static_cast<T>(edges.size());
        }

        btm::AABB<T> getBoundingBox() {
            if (vertices.size() == 0) {
                return btm::AABB<T>(); // empty bounding box
            }
            T min_x = std::numeric_limits<T>::max();
            T min_y = std::numeric_limits<T>::max();
            T min_z = std::numeric_limits<T>::max();
            T max_x = std::numeric_limits<T>::lowest();
            T max_y = std::numeric_limits<T>::lowest();
            T max_z = std::numeric_limits<T>::lowest();

            for (const auto& v_pair : vertices) {
                const btm::basevec3<T>& coords = v_pair.position;
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
            btm::AABB<T> bbox({ min_x, min_y, min_z }, { max_x, max_y, max_z });
            return bbox;
        }

        void translate(const btm::basevec3<T>& offset) {
            for (auto& vert : vertices) {
                btm::basevec3<T>& coords = vert.position;
                coords = coords + offset;
            }
            for (auto& face : faces) {
                face.translate(offset);
            }
        }

        void flip_all_faces() {
            for (auto& face : faces) {
                face.flip();
            }
            // curvatures are invalidated by flipping the faces, so we clear them to force recomputation
            vertex_curvatures.clear();
            // Recompute face normals after flipping
            calculate_face_normals();
            calculate_vertex_normals();
        }

        void build_adjacency() {
            vertex_adjacency.clear();
            vertex_adjacency.resize(vertices.size());

            face_adjacency.clear();
            face_adjacency.resize(faces.size());

            size_t num_faces = faces.size();
            for (std::uint32_t f = 0; f < num_faces; ++f) {
                const auto& face = faces[f];
                std::uint32_t vs[3] = { face.v0, face.v1, face.v2 };
                for (int i = 0; i < 3; ++i) {
                    auto v = vs[i];
                    auto vn = vs[(i + 1) % 3];

                    VertexAdjacency& va1 = vertex_adjacency[v];
                    VertexAdjacency& va2 = vertex_adjacency[vn];

                    va1.incident_faces.push_back(f);
                    va1.neighbor_vertices.push_back(vn);
                    va2.incident_faces.push_back(f);
                    va2.neighbor_vertices.push_back(v);
                }

                adjacent_faces(f, face_adjacency[f]);
            }
            // now remove duplicate neighbor vertices and faces for each vertex
            for (auto& va : vertex_adjacency) {
                std::sort(va.neighbor_vertices.begin(), va.neighbor_vertices.end());
                va.neighbor_vertices.erase(std::unique(va.neighbor_vertices.begin(), va.neighbor_vertices.end()), va.neighbor_vertices.end());

                std::sort(va.incident_faces.begin(), va.incident_faces.end());
                va.incident_faces.erase(std::unique(va.incident_faces.begin(), va.incident_faces.end()), va.incident_faces.end());
            }
        }

        void calculate_corner_angles() {
            for (auto& face : faces) {
                const auto& v0 = vertices[face.v0].position;
                const auto& v1 = vertices[face.v1].position;
                const auto& v2 = vertices[face.v2].position;
                face.angles[0] = corner_angle(v0, v1, v2);
                face.angles[1] = corner_angle(v1, v2, v0);
                face.angles[2] = corner_angle(v2, v0, v1);
            }
        }

        void calculate_edges_attributes() {
            for (const auto& edge_pair : edges) {
                const EdgeExplicit& edge = edge_pair.second;
                if (edge.is_boundary()) {
                    vertices[edge.v0()].is_boundary = true;
                    vertices[edge.v1()].is_boundary = true;
                }
            }
        }

        void build_attributes() {
            calculate_face_attributes();  // compute face normals, centers, and areas
            calculate_edges_attributes(); // compute boundary flags for vertices based on edges
            calculate_corner_angles();    // compute corner angles for each face
            compute_vertex_attributes();  // compute vertex normals based on face normals and corner angles
            compute_voronoi_areas();      // compute Voronoi areas for each vertex
        }

        // some functions to add vertices and faces are added here for
        // convenience although all the data members are public, we should still
        // provide some helper functions to maintain consistency and avoid errors
        // when adding data to the mesh. For example, when adding a face, we
        // should ensure that the vertex indices are valid and that the adjacency
        // information is updated accordingly.
        std::uint32_t add_vertex(size_t id, const btm::basevec3<T>& position) {
            vertices.push_back({ position });
            return vertices.size() - 1; // return the index of the newly added vertex
        }

        std::uint32_t add_face(const FaceExplicit<T>& face) {
            faces.push_back(face);
            return faces.size() - 1; // return the index of the newly added face
        }

        std::uint32_t add_face(std::uint32_t v0, std::uint32_t v1, std::uint32_t v2, std::uint32_t base) {
            return add_face({ v0 - base, v1 - base, v2 - base });
        }

        std::uint32_t add_face(std::uint32_t v0, std::uint32_t v1, std::uint32_t v2, std::uint32_t v3, std::uint32_t base) {
            // For quads, we can split them into two triangles (v0, v1, v2) and (v0, v2, v3)
            std::uint32_t f1 = add_face({ v0 - base, v1 - base, v2 - base });
            std::uint32_t f2 = add_face({ v0 - base, v2 - base, v3 - base });
            return f1; // return the index of the first triangle face
        }

        //std::uint32_t face_count() const { return faces.size(); }

        const FaceExplicit<T>& get_face(std::uint32_t index) const {
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

                FaceExplicit<T>& face = faces[f];
                int nvF = 3; // Since it's a triangle

                for (int e = 0; e < nvF; e++) {
                    int a = face.v(e);
                    int b = face.v((e + 1) % nvF);

                    edge_key key = EdgeExplicit::make_edge_key(a, b);
                    auto* edge = find_edge(key);
                    if (!edge) continue; // edge not found, skip

                    for (auto& face_id : edge->incident_faces) {
                        if (face_id == f) continue; // skip the current face
                        if (face_id == (std::uint32_t)-1) continue; // invalid face index, skip
                        FaceExplicit<T>& adj_face = faces[face_id];
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
        T component_signed_volume() {
            T vol = 0.0;
            for (FaceExplicit<T>& face : faces) {
                int nv = 3; // triangles only

                auto tri_vol = [&](int i0, int i1, int i2) {
                    btm::basevec3<T> a = vertices[i0].position;
                    btm::basevec3<T> b = vertices[i1].position;
                    btm::basevec3<T> c = vertices[i2].position;

                    return btm::signedVolume(btm::Triangle<T>{a, b, c});
                    };

                vol += tri_vol(face.v0, face.v1, face.v2);
            }

            return vol;
        }
    };
} // namespace btm

#endif // __mesh_explicit_h__
