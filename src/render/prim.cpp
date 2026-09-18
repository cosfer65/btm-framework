#include "glew.h"
#include "prim.h"

namespace btm {
    /**
     * @brief Creates a gl_prim object from the provided mesh data.
     * Initializes OpenGL buffers for vertex positions, normals, and indices.
     * @param mesh Pointer to mesh_data containing geometry information.
     * @param drmode OpenGL draw mode (default: GL_FILL).
     * @param dr_el Whether to use element drawing (default: true).
     */
    void gl_prim::create_from_mesh(mesh_data* mesh, GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        if (mesh->num_vertices == 0)
            return;

        clear_vao();

        draw_elements = dr_el;
        draw_mode = drmode;

        int idx = 0;

        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);

        // Calculate the sizes of vertex, normal, color, and index data in bytes
        // trusting sizeof(fvec3) == 3 * sizeof(float) is wrong, so we calculate the sizes explicitly
        size_t vertex_size = 3 * sizeof(float);
        size_t normal_size = 3 * sizeof(float);
        size_t color_size = 3 * sizeof(float);
        size_t index_size = sizeof(unsigned int);

        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, mesh->num_vertices * vertex_size, (float*)mesh->vertices.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(idx);
        glVertexAttribPointer(idx, 3, GL_FLOAT, GL_FALSE, vertex_size, (void*)0);
        idx++;

        if (mesh->num_normals > 0) {
            glGenBuffers(1, &normals_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
            glBufferData(GL_ARRAY_BUFFER, mesh->num_normals * normal_size, (float*)mesh->normals.data(), GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(idx);
            glVertexAttribPointer(idx, 3, GL_FLOAT, GL_FALSE, normal_size, (void*)0);
        }
        idx++;

        if (mesh->num_curvatures > 0) {
            glGenBuffers(1, &colors_buffer);
            glBindBuffer(GL_ARRAY_BUFFER, colors_buffer);
            glBufferData(GL_ARRAY_BUFFER, mesh->num_curvatures * color_size, (float*)mesh->curvatures.data(), GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(idx);
            glVertexAttribPointer(idx, 3, GL_FLOAT, GL_FALSE, color_size, (void*)0);
        }
        idx++;

        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->num_indices * index_size, (unsigned int*)mesh->indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);

        num_vertices = mesh->num_vertices; ///< Number of vertices in the mesh (required for glDrawArrays).
        num_indices = mesh->num_indices;  ///< Number of indices in the mesh (required for glDrawElements).
        num_normals = mesh->num_normals;  ///< Number of normals in the mesh.
        num_curvatures = mesh->num_curvatures; ///< Number of curvature values in the mesh (if available).
    }

    /**
     * @class gl_ucs
     * @brief Represents a UCS (Universal Coordinate System) primitive for rendering axes.
     * Inherits from gl_prim.
     */
    class gl_ucs :public gl_prim {
    protected:
    public:
        /**
         * @brief Default constructor for gl_ucs.
         */
        gl_ucs() {}
        /**
         * @brief Destructor for gl_ucs.
         */
        virtual ~gl_ucs() {}
        /**
         * @brief Renders the UCS axes using the provided shader.
         * Draws three colored axes (X: red, Y: green, Z: blue) using GL_LINES.
         * @param _shader Pointer to the shader used for rendering.
         */
        inline virtual void render(gl_shader* _shader) {
            if (!vertex_array) return;
            _shader->set_uniform("object_or_vertex_color", 0);

            // position object
            fmat4 ob_matrix = tmat * rmat * smat;
            ob_matrix = view_matrix * ob_matrix;

            // pass transformation to shader
            _shader->set_uniform("model", ob_matrix);

            glBindVertexArray(vertex_array);
            unsigned int point_count = (unsigned int)num_indices / 3; // divide the point count by 3 to get the number of points for each axis
            // _shader->set_vec3("object_color", fvec3(1, 0, 0));
            glDrawElements(GL_LINES, point_count, GL_UNSIGNED_INT, 0);
            // _shader->set_vec3("object_color", fvec3(0, 1, 0));
            glDrawElements(GL_LINES, point_count, GL_UNSIGNED_INT, (const void*)(point_count * sizeof(unsigned int)));
            // _shader->set_vec3("object_color", fvec3(0, 0, 1));
            glDrawElements(GL_LINES, point_count, GL_UNSIGNED_INT, (const void*)(2 * point_count * sizeof(unsigned int)));
            glBindVertexArray(0);
        }
    };

    static mesh_data* create_UCS_mesh() {
        mesh_data* ms = new mesh_data;
        // the center of the UCS is at the origin
        // x->red
        ms->add_vertex(fvec3(0, 0, 0));
        ms->add_vertex(fvec3(1, 0, 0));
        ms->add_vertex(fvec3(0.8f, 0.2f, 0));
        ms->add_vertex(fvec3(0.8f, -0.2f, 0));
        ms->add_indices(0, 1);
        ms->add_indices(1, 2);
        ms->add_indices(1, 3);
        for (int i = 0; i < 4; ++i) ms->add_color(fvec3(1.f, 0.f, 0.f));

        // y->green
        ms->add_vertex(fvec3(0, 0, 0));
        ms->add_vertex(fvec3(0, 1, 0));
        ms->add_vertex(fvec3(0.2f, 0.8f, 0));
        ms->add_vertex(fvec3(-0.2f, 0.8f, 0));
        ms->add_indices(4, 5);
        ms->add_indices(5, 6);
        ms->add_indices(5, 7);
        for (int i = 0; i < 4; ++i) ms->add_color(fvec3(0.f, 1.f, 0.f));

        // z->blue
        ms->add_vertex(fvec3(0, 0, 0));
        ms->add_vertex(fvec3(0, 0, 1));
        ms->add_vertex(fvec3(0, 0.2f, 0.8f));
        ms->add_vertex(fvec3(0, -0.2f, 0.8f));
        ms->add_indices(8, 9);
        ms->add_indices(9, 10);
        ms->add_indices(9, 11);
        for (int i = 0; i < 4; ++i) ms->add_color(fvec3(0.f, 0.f, 1.f));

        return ms;
    }

    /**
     * @brief Creates a UCS (Universal Coordinate System) primitive.
     * Generates a gl_mesh representing the UCS and returns a gl_ucs object.
     * @param drmode OpenGL draw mode (default: GL_FILL).
     * @param dr_el Whether to use element drawing (default: true).
     * @return Pointer to the created gl_prim (as gl_ucs).
     */
    gl_prim* create_UCS(GLenum drmode /*= GL_FILL*/, bool dr_el /*= true*/) {
        std::unique_ptr<mesh_data> mdata(create_UCS_mesh());
        gl_prim* p = new gl_ucs();
        p->create_from_mesh(mdata.get(), drmode, dr_el);
        p->set_draw_mode(drmode);
        return p;
    }
}