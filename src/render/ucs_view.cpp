#include "ucs_view.h"
#include "camera.h"
#include "shaders.h"
#include "light.h"
#include "prim.h"

#include "matrix.h"

namespace btm {
    /// @brief Private implementation details for `UCS_view`.
    ///
    /// This structure encapsulates the resources required by `UCS_view`, including:
    /// - `m_view`: viewport configuration (projection, size, position).
    /// - `m_cam`: camera controlling the UCS view orientation and position.
    /// - `m_shader`: shader program used to render the UCS primitive.
    /// - `m_light`: simple spotlight used to illuminate the UCS.
    /// - `m_ucs`: the actual UCS `gl_prim` (e.g., axis triad).
    struct UCS_view_private {
        std::unique_ptr<gl_camera> m_cam;                ///< gl_camera for scene viewing
        gl_shader* m_shader;             ///< Shader program for rendering

        std::unique_ptr<gl_light> m_light;               ///< Dedicated light for the UCS widget
        std::unique_ptr<gl_prim> m_ucs;                  ///< User coordinate system visualization
    };

    /// @class UCS_view
    /// @brief Small, self-contained view that renders a user coordinate system widget.
    ///
    /// `UCS_view` manages its own viewport, camera, shader, light, and UCS primitive. It is
    /// typically rendered into a fixed corner of the main window as an orientation helper
    /// (similar to axis widgets in CAD/3D tools). The user can rotate the UCS via
    /// `rotate_ucs_by` or `rotate_ucs_to`.
    UCS_view::UCS_view() {
        // Allocate the private implementation and initialize the viewport container.
        m_private_data = new UCS_view_private;
        m_rotation.loadIdentity(); // Initialize the user rotation to identity (no rotation).
        m_private_data->m_cam.reset(new gl_camera(fvec3(0, 0, 30), fvec3(0, 0, 0), fvec3(0, 1, 0)));
    }

    UCS_view::~UCS_view() {
        // Destroy the PIMPL container; smart pointers inside clean up their resources.
        delete m_private_data;
    }

    // leep these shaders sources here to minimize dependencies ftom external files
    static const char* ucs_vertex_shader = {
        "#version 330 core\n\
        layout(location = 0) in vec3 aPos;\n\
        layout(location = 2) in vec3 aColor;\n\
        uniform mat4 model;\n\
        uniform mat4 view;\n\
        uniform mat4 projection;\n\
        out vec3 oColor;\n\
        void main(){\n\
            gl_Position = projection * view * model * vec4(aPos, 1.0);\n\
            oColor = aColor;\n\
        }"
    };
    static const char* ucs_fragment_shader = {
        "#version 330 core\n\
        in vec3 oColor;\n\
        out vec4 color;\n\
        uniform vec3 object_color;\n\
        void main() {\n\
            color = vec4(oColor,1);\n\
        }"
    };

    /// @brief Initializes the UCS view resources (projection, camera, light, shader, and UCS).
    ///
    /// This must be called once before any calls to `render()`. It:
    /// - Configures a perspective projection on the internal viewport.
    /// - Creates a camera positioned along +Z looking at the origin.
    /// - Sets up a white spotlight illuminating the UCS from above/side.
    /// - Loads the mesh tools vertex/fragment shaders.
    /// - Creates and positions the UCS primitive at the world origin with a default scale.
    void UCS_view::initialize() {
        m_private_data->m_light.reset(new gl_light());
        m_private_data->m_shader = create_shader_s(ucs_vertex_shader, ucs_fragment_shader);

        m_private_data->m_ucs.reset(create_UCS());
        m_private_data->m_ucs->move_to(0, 0, 0);
        m_private_data->m_ucs->rotate_to(0, 0, 0);
        m_private_data->m_ucs->set_scale(fvec3(6.f));
    }

    /// @brief Renders the UCS widget into its configured viewport.
    ///
    /// This sets the viewport, binds the UCS shader, applies the UCS-specific light, uploads
    /// the combined camera and projection matrices, and finally draws the UCS primitive. The
    /// shader uniform `object_or_vertex_color` is set to 0 to indicate that object-level
    /// color should be used instead of per-vertex color.
    void UCS_view::render() {
        m_private_data->m_cam->set_viewport();
        m_private_data->m_shader->use();
        m_private_data->m_light->apply(m_private_data->m_shader);
        m_private_data->m_cam->apply(m_private_data->m_shader);
        m_private_data->m_ucs->view_matrix = m_rotation; // apply user rotation
        m_private_data->m_ucs->render(m_private_data->m_shader);
        m_private_data->m_shader->end();
    }

    /// @brief Updates the UCS viewport placement and aspect ratio.
    /// @param width  New window width (currently ignored).
    /// @param height New window height (currently ignored).
    ///
    /// Currently this implementation pins the UCS viewport to the origin (0,0) and forces a
    /// fixed aspect ratio of 1:1 (100x100). The `width` and `height` parameters are not yet
    /// used, but are kept for a future implementation that may position and scale the UCS
    /// relative to the main window size.
    void UCS_view::resize_window(int width, int height) {
        m_private_data->m_cam->set_aspect(150, 150);
    }

    /// @brief Sets the user-defined rotation for the UCS widget.
    /// @param R Rotation matrix to apply to the UCS view.
    ///
    /// This function replaces the current user rotation matrix used when rendering
    /// the UCS widget. The provided matrix `R` is stored in `m_rotation` and is
    /// applied in `UCS_view::render()` via `m_ucs->view_matrix`, allowing external
    /// code to control the orientation of the UCS (e.g., to match the main scene
    /// camera orientation or respond to user input).
    void UCS_view::set_user_rotation(const fmat4& R) {
        m_rotation = R;
    }
}