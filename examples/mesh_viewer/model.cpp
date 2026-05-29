#include "model.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "matrix.h"

using namespace btm;

static void expand_bbox(basematrix<double, 2, 3>& bbox, const basematrix<double, 2, 3>& part_bbox) {
    for (size_t i = 0; i < 3; ++i) {
        if (part_bbox(0, i) < bbox(0, i)) {
            bbox(0, i) = part_bbox(0, i);
        }
        if (part_bbox(1, i) > bbox(1, i)) {
            bbox(1, i) = part_bbox(1, i);
        }
    }
}

static dvec3 bbox_center(const basematrix<double, 2, 3>& bbox) {
    return dvec3(
        (bbox(0, 0) + bbox(1, 0)) * 0.5f,
        (bbox(0, 1) + bbox(1, 1)) * 0.5f,
        (bbox(0, 2) + bbox(1, 2)) * 0.5f
    );
}

static void recalculate_model(cModel* mdl) {
    basematrix<double, 2, 3> bbox(0);
    for (auto part : mdl->m_parts) {
        part->recalculateMesh();
        basematrix<double, 2, 3> part_bbox = part->getBoundingBox();
        expand_bbox(bbox, part_bbox);
    }
    dvec3 bbcenter = bbox_center(bbox);
    for (auto part : mdl->m_parts) {
        part->breakQuads();
        part->translate(dvec3(-bbcenter.x(), -bbcenter.y(), -bbcenter.z()));
    }
}

static bool load_obj(const std::string& fnm, cModel* mdl) {
    std::string line;
    std::ifstream mdl_file(fnm);
    size_t vertex_count = 1;
    size_t face_count = 1;
    btm::mesh<double>* mesh = nullptr;

    if (mdl_file.is_open()) {
        while (std::getline(mdl_file, line)) {
            auto tokens = splitString(line);
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
                mesh = new btm::mesh<double>();
                mdl->add_part(mesh);
            }
            else if (tokens[0] == "v") {
                // add vertex
                double x = atof(tokens[1].c_str());
                double y = atof(tokens[2].c_str());
                double z = atof(tokens[3].c_str());
                mesh->addVertex(vertex_count, btm::dvec3(x, y, z));
                ++vertex_count;
            }
            else if (tokens[0] == "f") {
                // add face
                auto v1 = splitString(tokens[1], '/');
                auto v2 = splitString(tokens[2], '/');
                auto v3 = splitString(tokens[3], '/');
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
                    auto v4 = splitString(tokens[4], '/');
                    size_t idx4 = static_cast<size_t>(std::stoull(v4[0]));
                    if (idx4 == 0 || idx4 >= vertex_count) {
                        // Index out of bounds; skip this face
                        continue;
                    }
                    mesh->addFace(idx1, idx2, idx3, idx4);
                }
                else {
                    mesh->addFace(idx1, idx2, idx3);
                }

                ++face_count;
            }
        }
        mdl_file.close();
        // finalize last mesh part
        recalculate_model(mdl);
    }
    else {
        return false;
    }

    return true;
}

cModel* load_mesh_model(const std::string& fnm) {
    if (file_extension(fnm) == "obj") {
        // load obj cModel
        cModel* mdl = new cModel();
        if (load_obj(fnm, mdl)) {
            mdl->set_name(fnm);
            return mdl;
        }
        return mdl;
    }
    return nullptr;
}