#ifndef __mesh_loader__
#define __mesh_loader__

#include <fstream>
#include <string>
#include <vector>

#include "vector.h"
#include "string_utils.h"

namespace btm {
    template <typename meshT, typename dataT>
    bool load_obj(const std::string& fnm, std::vector<meshT*>& meshes) {
        std::string line;
        std::ifstream mdl_file(fnm);
        size_t vertex_count = 1;
        size_t face_count = 1;
        meshT* mesh = nullptr;
        int vcount_start = 0; // to handle multiple "o" sections in the OBJ file, we need to keep track of the starting vertex index for each section

        if (mdl_file.is_open()) {
            std::vector<std::string> tokens;
            while (std::getline(mdl_file, line)) {
                tokens.clear();
                splitString(line.c_str(), tokens);
                if (tokens.empty()) {
                    continue; // skip empty lines
                }
                /*
                labels:
                "mtllib" -> material library
                "usemtl" -> use material
                "vn" -> vertex normal (they are not calculated as normal to the surface, and thus useless for our calculations)
                "vt" -> vertex texture coordinate (they are only for texture mapping, useless for our calculations)
                are ignored
                */
                if (tokens[0] == "o") {
                    if (mesh != nullptr) {
                        // finalize previous mesh part
                        // mesh->average_edge_length = mesh->total_edge_length / mesh->getEdges().size();
                    }
                    mesh = new meshT();
                    meshes.push_back(mesh);
                    vcount_start = vertex_count - 1;
                    continue;
                }
                if (tokens[0] == "v" && mesh == nullptr) {
                    mesh = new meshT();
                    meshes.push_back(mesh);
                    vcount_start = vertex_count - 1;
                }
                if (tokens[0] == "v") {
                    // add vertex
                    dataT x = (dataT)atof(tokens[1].c_str());
                    dataT y = (dataT)atof(tokens[2].c_str());
                    dataT z = (dataT)atof(tokens[3].c_str());
                    mesh->add_vertex(vertex_count, btm::basevec3<dataT>(x, y, z));
                    ++vertex_count;
                }
                else if (tokens[0] == "f") {
                    // add face
                    std::vector<std::string> v1, v2, v3;
                    splitString(tokens[1].c_str(), v1, '/');
                    splitString(tokens[2].c_str(), v2, '/');
                    splitString(tokens[3].c_str(), v3, '/');
                    size_t idx1 = static_cast<size_t>(std::stoull(v1[0]));
                    size_t idx2 = static_cast<size_t>(std::stoull(v2[0]));
                    size_t idx3 = static_cast<size_t>(std::stoull(v3[0]));
                    if (idx1 == 0 || idx2 == 0 || idx3 == 0) {
                        // OBJ indices are 1-based; zero is invalid
                        continue;
                    }
                    if (idx1 >= vertex_count || idx2 >= vertex_count || idx3 >= vertex_count) {
                        // Index out of bounds; skip this face
                        continue;
                    }
                    if (tokens.size() > 4) {
                        // More than 3 vertices per face
                        std::vector<std::string> v4;
                        splitString(tokens[4].c_str(), v4, '/');
                        size_t idx4 = static_cast<size_t>(std::stoull(v4[0]));
                        if (idx4 == 0 || idx4 >= vertex_count) {
                            // Index out of bounds; skip this face
                            continue;
                        }
                        mesh->add_face(idx1, idx2, idx3, idx4, vcount_start+1);
                    }
                    else {
                        mesh->add_face(idx1, idx2, idx3, vcount_start+1);
                    }

                    ++face_count;
                }
            }
            mdl_file.close();
        }
        else {
            return false;
        }

        return true;
    }
}

#endif // __mesh_loader__
