#ifndef __model_h__
#define __model_h__

#include <vector>
#include <string>

#include "mesh.h"

class cModel {
    std::string m_name; ///< Name of the model, used for identification and display purposes.
public:
    std::vector<btm::mesh<double>*> m_parts;
    ~cModel() {
        cleanUp();
    }
    void cleanUp() {
        for (auto part : m_parts) {
            delete part;
        }
        m_parts.clear();
    }
    void add_part(btm::mesh<double>* part) {
        m_parts.push_back(part);
    }
    const std::string& get_name() const {
        return m_name;
    }
    void set_name(const std::string& name) {
        m_name = name;
    }
};

cModel* load_mesh_model(const std::string& fnm);

#endif //__model_h__
