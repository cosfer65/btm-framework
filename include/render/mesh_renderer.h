#ifndef __mesh_renderer_h__
#define __mesh_renderer_h__

#include "mesh_explicit.h"
#include "prim.h"
#include "shaders.h"

namespace btm
{
    // A simple mesh renderer that uses OpenGL immediate mode to render the triangles of a MeshExplicit.
    // This is a very basic implementation for demonstration purposes. In a real application,
    // we will use modern OpenGL techniques such as vertex buffers and shaders for better performance and flexibility.
    // The MeshRenderer class is templated on the type T, which should match the type used in MeshExplicit (e.g., float).
    // The render function iterates over the triangles in the mesh, retrieves the vertex positions, and issues OpenGL commands to draw them.
    template <typename T>
    class MeshRenderer
    {
        std::unique_ptr<gl_prim> m_prim; // a gl_prim to store the mesh data for more efficient rendering in a real application.
        void collect_mesh_data(const MeshExplicit<T>& mesh, mesh_data& mdata) {
            // This function converts the MeshExplicit data into a flat format suitable for OpenGL rendering.
            // It iterates over the triangles in the mesh and extracts vertex positions to fill the mesh_data structure.
            size_t index = 0;
            size_t cur_face = 0;
            for (const auto& face : mesh.faces) {
                const auto& v0 = mesh.vertices[face.v0].position;
                const auto& v1 = mesh.vertices[face.v1].position;
                const auto& v2 = mesh.vertices[face.v2].position;
                const auto& norm = mesh.attributes.face_normals[cur_face]; // assuming one normal per face for flat shading

                mdata.vertices.push_back(static_cast<float>(v0.x()));
                mdata.vertices.push_back(static_cast<float>(v0.y()));
                mdata.vertices.push_back(static_cast<float>(v0.z()));

                mdata.normals.push_back(static_cast<float>(norm.x()));
                mdata.normals.push_back(static_cast<float>(norm.y()));
                mdata.normals.push_back(static_cast<float>(norm.z()));

                mdata.indices.push_back(static_cast<unsigned int>(index));
                ++index;

                mdata.vertices.push_back(static_cast<float>(v1.x()));
                mdata.vertices.push_back(static_cast<float>(v1.y()));
                mdata.vertices.push_back(static_cast<float>(v1.z()));

                mdata.normals.push_back(static_cast<float>(norm.x()));
                mdata.normals.push_back(static_cast<float>(norm.y()));
                mdata.normals.push_back(static_cast<float>(norm.z()));

                mdata.indices.push_back(static_cast<unsigned int>(index));
                ++index;

                mdata.vertices.push_back(static_cast<float>(v2.x()));
                mdata.vertices.push_back(static_cast<float>(v2.y()));
                mdata.vertices.push_back(static_cast<float>(v2.z()));

                mdata.normals.push_back(static_cast<float>(norm.x()));
                mdata.normals.push_back(static_cast<float>(norm.y()));
                mdata.normals.push_back(static_cast<float>(norm.z()));

                mdata.indices.push_back(static_cast<unsigned int>(index));
                ++index;
            }
            mdata.num_vertices = mdata.vertices.size();
            mdata.num_normals = mdata.normals.size();
            mdata.num_indices = mdata.indices.size();
        }
    public:
        /**
         * GLenum draw_mode sets the polygon rasterization mode.
         * GL_FILL, GL_LINE, GL_POINT
         */
        MeshRenderer(const MeshExplicit<T>& mesh, GLenum draw_mode = GL_LINE) {
            // we create a gl_prim and upload the mesh data to the GPU for efficient rendering.
            mesh_data md;
            collect_mesh_data(mesh, md);
            auto prim = std::make_unique<gl_prim>();
            prim->create_from_mesh(&md, draw_mode);
            m_prim = std::move(prim);
        }

        void rotate_by(float anglex, float angley, float anglez) {
            // This function applies a rotation transformation to the mesh.
            m_prim->rotate_by(fvec3(anglex, angley, anglez));
        }

        void render(gl_shader* _shader) const
        {
            m_prim->render(_shader);
        }
    };
} // namespace btm

#endif //__mesh_renderer_h__
