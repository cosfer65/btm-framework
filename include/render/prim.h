#pragma once

#include "transformations.h"
#include "gl_mesh.h"
#include "material.h"
#include "shaders.h"

namespace btm {
    class gl_prim {
    protected:
        // OpenGL handles for the vertex array object and vertex buffer objects
        GLuint vertex_array = 0;
        // buffer for vertex positions
        GLuint vertex_buffer = 0;
        // buffer for triangle/lines etc indices
        GLuint index_buffer = 0;
        // buffer for vertex normals
        GLuint normals_buffer = 0;
        // buffer for vertex colors, curvatures and other per-vertex attributes
        GLuint colors_buffer = 0;
        // not used so disabled for now, but could be used for texture coordinates
#if 0
        // buffer for texture coordinates
        GLuint tex_coords_buffer = 0;
        // buffer for tangents & bitangents
        // they are used for Physically Based Rendering (PBR) and normal mapping,
        // they are optional and will be used if the mesh has them
        GLuint tangents_buffer = 0;
        GLuint bitangents_buffer = 0;
#endif

        fvec3 object_color = fvec3(0.5f, 0.8f, 0.2f); // Color of the object

        size_t num_indices = 0;  ///< Number of indices in the mesh (required for glDrawElements).
        size_t num_vertices = 0; ///< Number of vertices in the mesh (required for glDrawArrays).
        size_t num_normals = 0;  ///< Number of normals in the mesh.
        size_t num_curvatures = 0; ///< Number of curvature values in the mesh (if available).

        GLenum draw_mode; // Specifies how polygons will be rasterized: GL_POINT, GL_LINE, GL_FILL.
        GLenum draw_type;  // Specifies the OpenGL primitive type for drawing: GL_TRIANGLES, GL_LINES, GL_PATCHES, etc.
        bool draw_elements; // If true, uses glDrawElements; otherwise, uses glDrawArrays.

        fvec3 position;           ///< Object position in world space.
        fvec3 scale;              ///< Object scale factors.
        fvec3 rotation;           ///< Object rotation angles (radians).

        fvec3 m_color;           ///< Base color of the primitive.
        cg_material* m_material; ///< Material for shading the primitive.
        GLuint m_texture;        ///< Texture handle for the primitive.

    public:
        fmat4 rmat;  ///< Local rotation matrix.
        fmat4 tmat;  ///< Translation matrix.
        fmat4 smat;  ///< Scaling matrix.
        bool force_black = false;        ///< If true, forces the primitive to render in black (e.g., for wireframe).
        fmat4 view_matrix; ///< View matrix for the primitive (optional, can be set externally).

        /**
         * @brief Constructs a gl_prim with default transformation and rendering state.
         */
        gl_prim() {
            scale = fvec3(1);
            position = fvec3(0);
            rotation = fvec3(0);
            draw_mode = GL_FILL;
            draw_type = GL_TRIANGLES;
            draw_elements = true;
            // matrices are col-major!
            rotate_to(rotation);
            smat = scale_matrix<float>(scale.x(), scale.y(), scale.z());
            move_to(position);
            m_texture = 0;
            m_material = nullptr;
            m_color = fvec3(0.8f, 0.8f, 0.8f);
            force_black = false;
            view_matrix.loadIdentity();
        }

        /**
         * @brief Virtual destructor.
         */
        virtual ~gl_prim() {
            clear();
        }

        void clear_vao() {
            glDeleteVertexArrays(1, &vertex_array);
            glDeleteBuffers(1, &vertex_buffer);
            glDeleteBuffers(1, &normals_buffer);
            glDeleteBuffers(1, &colors_buffer);
            // glDeleteBuffers(1, &tex_coords_buffer);
            glDeleteBuffers(1, &index_buffer);
            // glDeleteBuffers(1, &tangents_buffer);
            // glDeleteBuffers(1, &bitangents_buffer);
            vertex_array = 0;
            vertex_buffer = 0;
            normals_buffer = 0;
            colors_buffer = 0;
            // tex_coords_buffer = 0;
            index_buffer = 0;
            // tangents_buffer = 0;
            // bitangents_buffer = 0;
            num_vertices = 0;
            num_normals = 0;
            num_indices = 0;
        }

        void clear_mesh_data() {
            // m_mesh_data.vertices.clear();
            // m_mesh_data.normals.clear();
            // m_mesh_data.indices.clear();
            // m_mesh_data.num_vertices = 0;
            // m_mesh_data.num_normals = 0;
            // m_mesh_data.num_indices = 0;
        }
        void clear() {
            clear_vao();
            clear_mesh_data();
        }

        /**
         * @brief Initializes the primitive from mesh data.
         * @param mesh Pointer to mesh data.
         * @param drmode Polygon rasterization mode (GL_FILL, GL_LINE, etc.).
         * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
         */
        virtual void create_from_mesh(mesh_data* mesh, GLenum drmode = GL_FILL, bool dr_el = true);

        /**
         * @brief Sets the polygon rasterization mode.
         * @param dm OpenGL polygon mode (GL_FILL, GL_LINE, GL_POINT).
         */
        void set_draw_mode(GLenum dm) {
            draw_mode = dm;
        }

        /**
         * @brief Sets the texture for the primitive.
         * @param tex OpenGL texture handle.
         */
        void set_texture(GLuint tex) {
            m_texture = tex;
        }

        /**
         * @brief Sets the material for the primitive.
         * @param m Pointer to cg_material.
         */
        void set_material(cg_material* m) {
            m_material = m;
        }

        void set_color(const fvec3& col) {
            m_color = col;
        }

        /**
         * @brief Gets the current material.
         * @return Pointer to cg_material.
         */
        const cg_material* material() {
            return m_material;
        }

        /**
         * @brief Renders the primitive using the provided shader.
         * @param _shader Pointer to the shader program.
         */
        virtual void render(gl_shader* _shader) {
            if (!vertex_array) return;

            _shader->set_uniform("object_color", m_color);

            // position object
            fmat4 model_matrix = tmat * rmat * smat;
            model_matrix = view_matrix * model_matrix;

            // pass transformation to shader
            _shader->set_uniform("model", model_matrix);

            glBindVertexArray(vertex_array);
            if (draw_elements)
            {
                // setup drawing
                if (draw_type == GL_TRIANGLES) {
                    glFrontFace(GL_CCW);
                    glPolygonMode(GL_FRONT_AND_BACK, draw_mode);
                    glDrawElements(draw_type, (unsigned int)num_indices, GL_UNSIGNED_INT, 0);
                }
                else if (draw_type == GL_LINES) {
                    glDrawElements(draw_type, (unsigned int)num_indices, GL_UNSIGNED_INT, 0);
                }
                else if (draw_type == GL_POINTS) {
                    glDrawArrays(GL_POINTS, 0, (GLsizei)num_indices);
                }
            }
            else
            {
                // setup drawing
                glPatchParameteri(GL_PATCH_VERTICES, 4);
                glPolygonMode(GL_FRONT_AND_BACK, draw_mode);
                glDrawArrays(GL_PATCHES, 0, (unsigned int)num_indices);
            }
            glBindVertexArray(0);
        }

        /**
         * @brief Sets the OpenGL primitive type for drawing.
         * @param dt OpenGL draw type (GL_TRIANGLES, GL_LINES, etc.).
         */
        void set_draw_type(GLenum dt) {
            draw_type = dt;
        }

        // virtual void step_simulation(float fElapsed) {}

        /**
         * @brief Sets position, scale, and rotation in one call.
         * @param _p Position vector.
         * @param _s Scale vector.
         * @param _r Rotation vector (radians).
         */
        void set_all(const fvec3& _p, const fvec3& _s, const fvec3& _r) {
            position = _p;
            scale = _s;
            rotation = _r;
            move_to(position);
            smat = scale_matrix<float>(scale.x(), scale.y(), scale.z());
            rotate_to(rotation);
        }

        /**
         * @brief Sets the rotation to the specified vector.
         * @param _r Rotation vector (radians).
         */
        void rotate_to(const fvec3& _r) {
            rotation = _r;
            rmat = rotation_matrix<float>(rotation.x(), rotation.y(), rotation.z());
        }

        /**
         * @brief Sets the rotation to the specified angles.
         * @param x Rotation around X axis (radians).
         * @param y Rotation around Y axis (radians).
         * @param z Rotation around Z axis (radians).
         */
        void rotate_to(float x, float y, float z) {
            rotate_to(fvec3(x, y, z));
        }

        /**
         * @brief Adds the specified vector to the current rotation.
         * @param _r Rotation vector to add (radians).
         */
        void rotate_by(const fvec3& _r) {
            rotation += _r;
            rotate_to(rotation);
        }

        /**
         * @brief Adds the specified angles to the current rotation.
         * @param x Rotation around X axis (radians).
         * @param y Rotation around Y axis (radians).
         * @param z Rotation around Z axis (radians).
         */
        void rotate_by(float x, float y, float z) {
            rotation += fvec3(x, y, z);
            rotate_to(rotation);
        }

        /**
         * @brief Sets the position to the specified vector.
         * @param _r Position vector.
         */
        void move_to(const fvec3& _r) {
            position = _r;
            tmat = translation_matrix<float>(position.x(), position.y(), position.z());
        }

        /**
         * @brief Sets the position to the specified coordinates.
         * @param x X coordinate.
         * @param y Y coordinate.
         * @param z Z coordinate.
         */
        void move_to(float x, float y, float z) {
            position = fvec3(x, y, z);
            move_to(position);
        }

        /**
         * @brief Adds the specified vector to the current position.
         * @param _r Position vector to add.
         */
        void move_by(const fvec3& _r) {
            position += _r;
            move_to(position);
        }

        /**
         * @brief Adds the specified values to the current position.
         * @param x X increment.
         * @param y Y increment.
         * @param z Z increment.
         */
        void move_by(float x, float y, float z) {
            position += fvec3(x, y, z);
            move_to(position);
        }

        /**
         * @brief Virtual method for creating the primitive geometry.
         * @param drmode Polygon rasterization mode.
         * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
         */
        virtual void create(GLenum drmode = GL_FILL, bool dr_el = true) {}

        /**
         * @brief Sets the scale vector.
         * @param _s Scale vector.
         */
        void set_scale(const fvec3& _s) {
            scale = _s;
            smat = scale_matrix<float>(scale.x(), scale.y(), scale.z());
        }

        /**
         * @brief Sets the scale factors.
         * @param x Scale along X axis.
         * @param y Scale along Y axis.
         * @param z Scale along Z axis.
         */
        void set_scale(float x, float y, float z) {
            set_scale(fvec3(x, y, z));
        }

        /**
         * @brief Sets the scale along the X axis.
         * @param _s Scale value.
         */
        void set_xscale(float _s) {
            scale.x() = _s;
            set_scale(scale);
        }

        /**
         * @brief Sets the scale along the Y axis.
         * @param _s Scale value.
         */
        void set_yscale(float _s) {
            scale.y() = _s;
            set_scale(scale);
        }

        /**
         * @brief Sets the scale along the Z axis.
         * @param _s Scale value.
         */
        void set_zscale(float _s) {
            scale.z() = _s;
            set_scale(scale);
        }

        /**
         * @brief Gets the current position vector.
         * @return Reference to position vector.
         */
        fvec3& get_position() {
            return position;
        }

        /**
         * @brief Gets the current scale vector.
         * @return Reference to scale vector.
         */
        fvec3& get_scale() {
            return scale;
        }

        /**
         * @brief Gets the current rotation vector.
         * @return Reference to rotation vector.
         */
        fvec3& get_rotation() {
            return rotation;
        }
    };

    /**
     * @brief Creates a gl_prim from a mesh.
     * @param ms Pointer to mesh.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */

    template <typename T>
    gl_prim* create_prim(btm::MeshExplicit<T>* ms, GLenum drmode = GL_LINE, bool dr_el = true) {
        if (!ms) return nullptr;
        mesh_data mdata;
        collect_mesh_data<T>(ms, mdata);
        gl_prim* prim = new gl_prim;
        prim->create_from_mesh(&mdata, drmode);
        prim->set_draw_mode(drmode);
        return prim;
    }

    /**
     * @brief Creates a gl_prim from a gl_mesh.
     * @param ms Pointer to gl_mesh.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_prim(mesh_data* ms, GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a Universal Coordinate System (XYZ axes with arrows).
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_UCS(GLenum drmode = GL_LINE, bool dr_el = true);
}
