#ifndef __primitives__
#define __primitives__

#include "transformations.h"
#include "gl_mesh.h"
#include "material.h"
#include "shaders.h"

namespace btm {
    /**
     * @brief Base class for OpenGL drawable primitives.
     *
     * Encapsulates mesh data, transformation matrices, material, and rendering state.
     * Provides methods for setting transformation, draw mode/type, and rendering.
     */
    class gl_prim {
    protected:
        mesh_data m_mesh_data;   ///< Mesh data for the primitive.
        GLuint vao;              ///< Vertex Array Object handle.

        /**
         * @brief Specifies how polygons will be rasterized.
         * Accepted values: GL_POINT, GL_LINE, GL_FILL.
         */
        GLenum draw_mode;

        /**
         * @brief Specifies the OpenGL primitive type for drawing.
         * Common values: GL_TRIANGLES, GL_LINES, GL_PATCHES, etc.
         */
        GLenum draw_type;

        /**
         * @brief If true, uses glDrawElements; otherwise, uses glDrawArrays.
         */
        bool draw_elements;

        fvec3 position;           ///< Object position in world space.
        fvec3 scale;              ///< Object scale factors.
        fvec3 rotation;           ///< Object rotation angles (radians).

        /**
         * @brief If nonzero, enables per-vertex color in the shader.
         */
        int use_vertex_color;

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
            vao = 0;
            scale = fvec3(1);
            position = fvec3(0);
            rotation = fvec3(0);
            draw_mode = GL_FILL;
            draw_type = GL_TRIANGLES;
            draw_elements = true;
            // matrices are row-major!
            rotate_to(rotation);
            smat = Scale<float>(scale.x(), scale.y(), scale.z());
            move_to(position);
            use_vertex_color = 0;
            m_texture = 0;
            m_material = nullptr;
            m_color = fvec3(0.8f, 0.8f, 0.8f);
            force_black = false;
            view_matrix.loadIdentity();
        }

        int get_use_vertex_color() const {
            return use_vertex_color;
        }

        int set_use_vertex_color(int v) {
            int ret = use_vertex_color;
            use_vertex_color = v;
            return ret;
        }

        /**
         * @brief Virtual destructor.
         */
        virtual ~gl_prim() {
            clear_vao();
        }

        void clear_vao() {
            if (vao) {
                glDeleteVertexArrays(1, &vao);
                vao = 0;
            }
        }

        void clear_mesh_data() {
            m_mesh_data.vertices.clear();
            m_mesh_data.normals.clear();
            m_mesh_data.indices.clear();
            m_mesh_data.num_vertices = 0;
            m_mesh_data.num_normals = 0;
            m_mesh_data.num_indices = 0;
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
            if (!vao) return;

            if (force_black) {
                _shader->set_vec3("object_color", fvec3(0, 0, 0));
            }
            else {
                _shader->set_vec3("object_color", m_color);
            }

            // position object
            fmat4 model_matrix = tmat * rmat * smat;
            model_matrix = model_matrix.transpose();    // convert to column wise for OpenGL!
            model_matrix = view_matrix * model_matrix;

            // pass transformation to shader
            _shader->set_mat4("model", model_matrix);

            glBindVertexArray(vao);
            if (draw_elements)
            {
                // setup drawing
                if (draw_type == GL_TRIANGLES) {
                    glFrontFace(GL_CCW);
                    glPolygonMode(GL_FRONT_AND_BACK, draw_mode);
                }
                glDrawElements(draw_type, (unsigned int)m_mesh_data.num_indices, GL_UNSIGNED_INT, 0);
            }
            else
            {
                // setup drawing
                glPatchParameteri(GL_PATCH_VERTICES, 4);
                glPolygonMode(GL_FRONT_AND_BACK, draw_mode);
                glDrawArrays(GL_PATCHES, 0, (unsigned int)m_mesh_data.num_indices);
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
            smat = Scale<float>(scale.x(), scale.y(), scale.z());
            rotate_to(rotation);
        }

        /**
         * @brief Sets the rotation to the specified vector.
         * @param _r Rotation vector (radians).
         */
        void rotate_to(const fvec3& _r) {
            rotation = _r;
            rmat = Rotation<float>(rotation.x(), rotation.y(), rotation.z());
        }

        /**
         * @brief Sets the rotation to the specified angles.
         * @param x Rotation around X axis (radians).
         * @param y Rotation around Y axis (radians).
         * @param z Rotation around Z axis (radians).
         */
        void rotate_to(float x, float y, float z) {
            rotation = fvec3(x, y, z);
            rotate_to(rotation);
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
            tmat = Translation<float>(position.x(), position.y(), position.z());
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
            smat = Scale<float>(scale.x(), scale.y(), scale.z());
        }

        /**
         * @brief Sets the scale factors.
         * @param x Scale along X axis.
         * @param y Scale along Y axis.
         * @param z Scale along Z axis.
         */
        void set_scale(float x, float y, float z) {
            scale = fvec3(x, y, z);
            set_scale(scale);
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
     // gl_prim* create_prim(mesh<float>* ms, GLenum drmode=GL_LINE, bool dr_el=true);
    template <typename T>
    gl_prim* create_prim(btm::mesh<T>* ms, GLenum drmode = GL_LINE, bool dr_el = true) {
        if (!ms) return nullptr;
        mesh_data mdata;
        collect_mesh_data<T>(ms, mdata);
        gl_prim* prim = new gl_prim;
        prim->create_from_mesh(&mdata, drmode);
        prim->set_draw_mode(drmode);
        return prim;
    }

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
    gl_prim* create_prim(gl_mesh* ms, GLenum drmode = GL_LINE, bool dr_el = true);

#if 0
    /**
     * @brief Creates a cone primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_cone(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a cube primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_cube(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a cylinder primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_cylinder(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a dodecahedron primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_dodecahedron(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates an icosahedron primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_icosahedron(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates an octahedron primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_octa(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a pentagonal primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_penta(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a plane primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_plane(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a sphere primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_sphere(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a tetrahedron primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_tetra(GLenum drmode = GL_LINE, bool dr_el = true);

    /**
     * @brief Creates a torus primitive.
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_torus(GLenum drmode = GL_LINE, bool dr_el = true);
#endif

    /**
     * @brief Creates a unit cube primitive.
     * Generates a mesh representing a unit cube and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_cube(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_cube<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit sphere primitive.
     * Generates a mesh representing a unit sphere and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_sphere(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_sphere<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit cylinder primitive.
     * Generates a mesh representing a unit cylinder and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_cylinder(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_cylinder<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit cone primitive.
     * Generates a mesh representing a unit cone and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_cone(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_cone<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit dodecahedron primitive.
     * Generates a mesh representing a unit dodecahedron and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_dodecahedron(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_dodecahedron<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit icosahedron primitive.
     * Generates a mesh representing a unit icosahedron and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_icosahedron(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_icosahedron<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit octahedron primitive.
     * Generates a mesh representing a unit octahedron and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_octa(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_octa<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit pentahedron primitive.
     * Generates a mesh representing a unit pentahedron and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_penta(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_penta<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit plane primitive.
     * Generates a mesh representing a unit plane and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_plane(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_plane<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit tetrahedron primitive.
     * Generates a mesh representing a unit tetrahedron and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_tetra(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_tetra<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a unit torus primitive.
     * Generates a mesh representing a unit torus and returns a gl_prim.
     * @param drmode OpenGL draw mode (default: GL_LINE).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim.
     */
    template <typename T, typename Tinput>
    gl_prim* create_torus(GLenum drmode = GL_LINE, bool dr_el = true) {
        std::unique_ptr<mesh<T>> ms(create_unit_torus<T, Tinput>());
        gl_prim* p = create_prim(ms.get(), drmode, dr_el);
        return p;
    }

    /**
     * @brief Creates a Universal Coordinate System (XYZ axes with arrows).
     * @param drmode Polygon rasterization mode.
     * @param dr_el If true, use glDrawElements; otherwise, use glDrawArrays.
     * @return Pointer to the created gl_prim.
     */
    gl_prim* create_UCS(GLenum drmode = GL_LINE, bool dr_el = true);
}

#endif // __primitives__
