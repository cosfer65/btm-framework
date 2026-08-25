#pragma once
#include <vector>
#include "mesh_explicit.h"
#include "feature_line.h"
#include "feature_line_params.h"
#include "base_definitions.h"

#include "utils.h"

namespace btm
{
    template <typename T>
    int chooseNextVertexAlongDirection(int currentVertex, const basevec3<T>& direction, const MeshExplicit<T>& mesh) {
        const auto& neighbors = mesh.vertex_adjacency[currentVertex].neighbor_vertices;
        if (neighbors.empty())
            return -1;

        const basevec3<T>& p0 = mesh.vertices[currentVertex].position;

        T bestScore = -std::numeric_limits<T>::infinity();
        int    bestNeighbor = -1;

        for (int nv : neighbors) {
            const basevec3<T>& p1 = mesh.vertices[nv].position;
            basevec3<T> edge = p1 - p0;

            T len = edge.length();
            if (len <= 1e-12)
                continue;

            basevec3<T> edgeDir = edge / len;
            T score = dot(edgeDir, direction); // alignment with principal direction

            if (score > bestScore) {
                bestScore = score;
                bestNeighbor = nv;
            }
        }

        // Require positive alignment to avoid going backwards or sideways
        if (bestScore <= 0.0)
            return -1;

        return bestNeighbor;
    }

    template <typename T>
    class FeatureLineExtractor
    {
    private:
        const MeshExplicit<T>& m_mesh;
        const CurvatureField<T>& m_curvature;
        FeatureLineParameters<T>  m_params;
        int num_vertices;
    public:
        FeatureLineExtractor(const MeshExplicit<T>& mesh, const CurvatureField<T>& curvature, const FeatureLineParameters<T>& params)
            : m_mesh(mesh), m_curvature(curvature), m_params(params) {
            num_vertices = static_cast<int>(mesh.num_vertices());
        }

        // --- Public API ---------------------------------------------------------

        std::vector<FeatureLine<T>> extractRidges() {
            // Extract only ridge lines
            std::vector<FeatureLine<T>> lines;
            const int V = static_cast<int>(m_mesh.vertices.size());

            std::vector<bool> visited(V, false);

            for (int v = 0; v < V; ++v)
            {
                if (!isRidgeVertex(v))
                    continue;
                if (visited[v])
                    continue;

                FeatureLine<T> line = traceCurvatureLine(v, /*ridge=*/true);
                if (!line.empty())
                {
                    // Mark vertices of this line as visited to avoid duplicates
                    for (int idx : line.vertices)
                        visited[idx] = true;

                    lines.push_back(std::move(line));
                }
            }

            return lines;
        }

        // Extract only valley lines
        std::vector<FeatureLine<T>> extractValleys() {
            std::vector<FeatureLine<T>> lines;
            const int V = static_cast<int>(m_mesh.vertices.size());

            std::vector<bool> visited(V, false);

            for (int v = 0; v < V; ++v)
            {
                if (!isValleyVertex(v))
                    continue;
                if (visited[v])
                    continue;

                FeatureLine line = traceCurvatureLine(v, /*ridge=*/false);
                if (!line.empty())
                {
                    for (int idx : line.vertices)
                        visited[idx] = true;

                    lines.push_back(std::move(line));
                }
            }

            return lines;
        }

        // Extract sharp creases (dihedral-angle based)
        std::vector<FeatureLine<T>> extractCreases() {
            std::vector<int> creaseEdges;

            const int E = static_cast<int>(m_mesh.edges.size()); // m_mesh.adjacency.edgeVertices.size());
            for (int e = 0; e < E; ++e)
            {
                T angle = computeDihedralAngle(e);
                if (angle < -5)
                    debug_out("dihedral angle = ", angle);
                if (angle >= m_params.dihedralAngleThreshold)
                    creaseEdges.push_back(e);
            }

            return buildCreaseLines(creaseEdges);
        }

        // Extract everything
        std::vector<FeatureLine<T>> extractAll() {
            std::vector<FeatureLine<T>> all;

            auto ridges = extractRidges();
            auto valleys = extractValleys();
            auto creases = extractCreases();

            all.reserve(ridges.size() + valleys.size() + creases.size());
            all.insert(all.end(), ridges.begin(), ridges.end());
            all.insert(all.end(), valleys.begin(), valleys.end());
            all.insert(all.end(), creases.begin(), creases.end());

            return all;
        }

    private:
        // --- Internal helpers ---------------------------------------------------

        // Ridge/valley tracing from a seed vertex
        FeatureLine<T> traceCurvatureLine(int seedVertex, bool ridge) {
            FeatureLine<T> line;

            if (seedVertex < 0 || seedVertex >= static_cast<int>(m_mesh.vertices.size()))
                return line;

            // Check seed validity
            if (ridge && !isRidgeVertex(seedVertex)) {
                //odprintf("return point 1");
                return line;
            }
            if (!ridge && !isValleyVertex(seedVertex)) {
                //odprintf("return point 2");
                return line;
            }
                

            // Forward and backward tracing to get a full polyline
            auto traceOneSide = [&](int startVertex, bool forward) -> std::vector<int>
                {
                    std::vector<int> path;
                    int current = startVertex;

                    for (int step = 0; step < m_params.maxTraceSteps; ++step)
                    {
                        path.push_back(current);

                        const basevec3<T>& d1 = m_curvature[current].kmin_dir;
                        basevec3<T> dir = forward ? d1 : (d1 * -1.0);

                        int next = chooseNextVertexAlongDirection(current, dir, m_mesh);
                        if (next < 0)
                            break;

                        // Stop if next vertex no longer satisfies ridge/valley criteria
                        if (ridge && !isRidgeVertex(next))
                            break;
                        if (!ridge && !isValleyVertex(next))
                            break;

                        current = next;
                    }

                    return path;
                };
            // Trace forward and backward
            std::vector<int> forwardPath = traceOneSide(seedVertex, true);
            std::vector<int> backwardPath = traceOneSide(seedVertex, false);

            // Combine: backward (reversed, excluding seed) + forward
            if (!forwardPath.empty())
            {
                // backwardPath starts at seed; we skip its first element to avoid duplication
                for (int i = static_cast<int>(backwardPath.size()) - 1; i >= 1; --i)
                    line.vertices.push_back(backwardPath[i]);

                for (int v : forwardPath)
                    line.vertices.push_back(v);
            }

            // Optionally fill points from vertices
            line.points.reserve(line.vertices.size());
            for (int v : line.vertices)
                line.points.push_back(m_mesh.vertices[v].position);

            // Enforce minimum size
            if (static_cast<int>(line.vertices.size()) < m_params.minLineSize)
                line.clear();

            return line;
        }

        // Check if vertex satisfies ridge/valley criteria
        bool isRidgeVertex(int v) const {
            if (v < 0 || v >= num_vertices)
                return false;

            T k = m_curvature[v].kmax;
            return std::abs(k) > m_params.ridge_threshold && k > 0.0;
        }
        bool isValleyVertex(int v) const {
            if (v < 0 || v >= num_vertices)
                return false;

            T k = m_curvature[v].kmax;
            return std::abs(k) > m_params.valley_threshold && k < 0.0;
        }

        // Compute dihedral angle for an edge
        T computeDihedralAngle(int edgeIndex) const {
            const auto& e = m_mesh.edge(edgeIndex);
            const auto& ef = e.incident_faces;

            int f0 = ef[0]; // face 0
            int f1 = ef[1]; // face 1

            // Boundary edge: no dihedral (or treat as 0)
            if (f0 < 0 || f1 < 0) {
                // odprintf("Warning: Edge %d is a boundary edge; dihedral angle undefined. Returning 0.\n", edgeIndex);
                return 0.0;
            }

            const basevec3<T>& n0 = m_mesh.faces[f0].normal;
            const basevec3<T>& n1 = m_mesh.faces[f1].normal;

            T dihedral = btm::dihedralAngle(n0, n1);
            return rtd<T>(dihedral);
        }

        // Compute dihedral angle for an edge
        T f_computeDihedralAngle(int edgeIndex) const {
            const auto& e = m_mesh.edge(edgeIndex);
            const auto& ef = e.incident_faces;

            int v0 = e.v0();
            int v1 = e.v1();
            int f0 = ef[0]; // face 0
            int f1 = ef[1]; // face 1

            // Boundary edge: no dihedral (or treat as 0)
            if (f0 < 0 || f1 < 0) {
                // odprintf("Warning: Edge %d is a boundary edge; dihedral angle undefined. Returning 0.\n", edgeIndex);
                return 0.0;
            }
            // Invalid edge: no dihedral
            if (v0 < 0 || v1 < 0) {
                // odprintf("Warning: Edge %d has invalid vertices; dihedral angle undefined. Returning 0.\n", edgeIndex);
                return 0.0;
            }
            const basevec3<T>& p0 = m_mesh.vertices[v0].position;
            const basevec3<T>& p1 = m_mesh.vertices[v1].position;

            const basevec3<T>& n0 = m_mesh.faces[f0].normal;
            const basevec3<T>& n1 = m_mesh.faces[f1].normal;

            basevec3<T> edgeDir = (p1 - p0).normalize();

            // Remove edge component from normals to get "face tilt" around edge
            basevec3<T> n0Proj = (n0 - edgeDir * dot(n0, edgeDir)).normalize();
            basevec3<T> n1Proj = (n1 - edgeDir * dot(n1, edgeDir)).normalize();

            T cosTheta = dot(n0Proj, n1Proj);
            cosTheta = std::max(T(-1), std::min(T(1), cosTheta));

            T theta = std::acos(cosTheta); // radians
            return rtd<T>(theta);
        }

        // Build crease lines from marked edges
        std::vector<FeatureLine<T>> buildCreaseLines(const std::vector<int>& creaseEdges) {
            std::vector<FeatureLine<T>> lines;
            
            const int E = static_cast<int>(m_mesh.edges.size());
            std::vector<bool> edgeUsed(E, false);

            // Mark crease edges
            for (int e : creaseEdges)
                if (e >= 0 && e < E)
                    edgeUsed[e] = false; // will be used as "available"

            // Build vertex -> incident crease edges map
            std::vector<std::vector<int>> vertexToCreaseEdges(m_mesh.vertices.size());
            for (int e : creaseEdges)
            {
                const auto& ev = m_mesh.edge(e);
                vertexToCreaseEdges[ev.v0()].push_back(e);
                vertexToCreaseEdges[ev.v1()].push_back(e);
            }

            // Helper: get next edge from a vertex that is still unused
            auto getUnusedEdgeFromVertex = [&](int v) -> int
                {
                    for (int e : vertexToCreaseEdges[v])
                    {
                        if (!edgeUsed[e])
                            return e;
                    }
                    return -1;
                };

            // For each crease edge, grow a polyline
            for (int eStart : creaseEdges)
            {
                if (edgeUsed[eStart])
                    continue;

                FeatureLine<T> line;

                const auto& evStart = m_mesh.edge(eStart);
                int vA = evStart.v0();
                int vB = evStart.v1();

                // We will grow from both ends
                auto growSide = [&](int startVertex, int otherVertex, std::vector<int>& outVertices)
                    {
                        int currentVertex = startVertex;
                        int prevVertex = otherVertex;

                        while (true)
                        {
                            outVertices.push_back(currentVertex);

                            int nextEdge = -1;
                            for (int e : vertexToCreaseEdges[currentVertex])
                            {
                                if (edgeUsed[e])
                                    continue;

                                const auto& ev = m_mesh.edge(e);
                                int v0 = ev.v0();
                                int v1 = ev.v1();

                                int nextVertex = (v0 == currentVertex) ? v1 :
                                    (v1 == currentVertex) ? v0 : -1;

                                if (nextVertex < 0 || nextVertex == prevVertex)
                                    continue;

                                nextEdge = e;
                                prevVertex = currentVertex;
                                currentVertex = nextVertex;
                                edgeUsed[e] = true;
                                break;
                            }

                            if (nextEdge < 0)
                                break;
                        }
                    };

                // Mark starting edge as used
                edgeUsed[eStart] = true;

                std::vector<int> leftVertices;
                std::vector<int> rightVertices;

                growSide(vA, vB, leftVertices);
                growSide(vB, vA, rightVertices);

                // Combine: left (reversed) + right
                FeatureLine<T> fl;
                for (int i = static_cast<int>(leftVertices.size()) - 1; i >= 0; --i)
                    fl.vertices.push_back(leftVertices[i]);
                for (int v : rightVertices)
                    fl.vertices.push_back(v);

                // Fill points
                fl.points.reserve(fl.vertices.size());
                for (int v : fl.vertices)
                    fl.points.push_back(m_mesh.vertices[v].position);

                if (static_cast<int>(fl.vertices.size()) >= m_params.minLineSize)
                    lines.push_back(std::move(fl));
            }

            return lines;
        }
    };
}