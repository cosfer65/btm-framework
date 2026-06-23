#pragma once

#include "matrix.h"

namespace btm {
    /// @brief Forward declaration of implementation details for `UCS_view`.
    ///
    /// The actual data and logic are hidden behind a PIMPL (pointer-to-implementation)
    /// to keep the header lightweight and reduce compile-time dependencies.
    struct UCS_view_private;

    /// @brief Manages visualization and interaction with a UCS (User Coordinate System) view.
    ///
    /// `UCS_view` is responsible for initializing, rendering, and updating the orientation
    /// of a local coordinate system, typically rendered as a small axis gizmo.
    class UCS_view {
        UCS_view_private* m_private_data; ///< Opaque pointer to implementation-specific data.
        btm::fmat4 m_rotation;                 ///< Current rotation matrix for the UCS, stored in column-major order.
    public:
        /// @brief Constructs an uninitialized UCS view.
        ///
        /// The underlying implementation data is allocated but not fully initialized
        /// until `initialize()` is called.
        UCS_view();

        /// @brief Destroys the UCS view and releases implementation data and GPU resources.
        ~UCS_view();

        /// @brief Initializes OpenGL resources and internal state required for rendering the UCS.
        ///
        /// Must be called before `render()` or any rotation methods are used.
        void initialize();

        /// @brief Renders the UCS indicator to the current OpenGL context.
        ///
        /// Typically called once per frame after the main scene has been drawn.
        void render();

        /// @brief Updates the UCS view to match a new window size.
        ///
        /// This is usually called from a window-resize callback to adjust viewport,
        /// projection, or placement of the UCS overlay.
        ///
        /// @param width  New width of the rendering surface, in pixels.
        /// @param height New height of the rendering surface, in pixels.
        void resize_window(int width, int height);

        /// @brief Sets an additional user-defined rotation for the UCS.
        ///
        /// This rotation is typically combined with the internally maintained
        /// UCS orientation (e.g., derived from camera or model transforms) to
        /// allow the user to manually adjust how the UCS gizmo is oriented.
        ///
        /// Use this to override or augment the default UCS orientation based on
        /// user interaction such as trackball rotation, arcball controls, or
        /// explicit matrix edits.
        ///
        /// @param R Rotation matrix to apply to the UCS, in the same coordinate
        ///          space and layout convention as `m_rotation` (column-major).
        void set_user_rotation(const btm::fmat4& R);
    };
}