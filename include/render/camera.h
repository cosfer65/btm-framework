/// \file gl_camera.h
/// \brief Declares the `btm::gl_camera` class, a simple OpenGL camera wrapper.

#ifndef __camera_h__
#define __camera_h__

#undef max

#include <algorithm>
#include "vector.h"
#include "gl_math.h"
#include "shaders.h"

namespace btm {

    /// \brief Simple perspective camera for OpenGL rendering.
    ///
    /// Encapsulates camera position, orientation and projection parameters,
    /// and provides convenience methods to build view/projection matrices,
    /// configure the OpenGL viewport and perform basic camera panning.
    class gl_camera {
        /// \brief Camera position in world space.
        btm::fvec3 location;

        /// \brief Point in world space the camera is looking at.
        btm::fvec3 target;

        /// \brief Up direction of the camera in world space.
        btm::fvec3 up = btm::fvec3(0, 1, 0);

        /// \brief Vertical field of view in radians.
        float fov = PI<float> / 6;

        /// \brief Distance to the near clipping plane.
        float nearPlane = 0.1f;

        /// \brief Distance to the far clipping plane.
        float farPlane = 1000.0f;

        /// \brief Aspect ratio of the viewport (width / height).
        float aspect = 1.f;

        /// \brief Window width and height in pixels.
        int width, height;  ///< Window width and height.

        /// \brief Left coordinate of the viewport within the window.
        int left = 0;

        /// \brief Bottom coordinate of the viewport within the window.
        int bottom = 0; ///< Viewport position within the window.

        bool dragging = false;           ///< Indicates whether the user is currently dragging with the mouse (for panning)
        int last_mouse_x = 0;            ///< Last recorded mouse X position (used for calculating deltas during dragging)
        int last_mouse_y = 0;            ///< Last recorded mouse Y position (used for calculating deltas during dragging)

    public:

        void begin_drag(int x, int y) {
            dragging = true;
            last_mouse_x = x;
            last_mouse_y = y;
        }
        void end_drag() {
            dragging = false;
        }
        void mouse_move(int x, int y) {
            if (dragging) {
                int deltax = x - last_mouse_x;
                int deltay = y - last_mouse_y;
                pan(float(deltax), float(deltay)); // adjust the camera position based on mouse movement
                last_mouse_x = x;
                last_mouse_y = y;
            }
        }

        /// \brief Constructs a default camera with uninitialized position/target.
        gl_camera() = default;

        /// \brief Constructs a camera with the given position, target and up vector.
        ///
        /// \param _location Camera position in world space.
        /// \param _target   Look-at target point in world space.
        /// \param _up       Up direction vector (defaults to +Y).
        gl_camera(const btm::fvec3& _location,
                  const btm::fvec3& _target,
                  const btm::fvec3& _up = btm::fvec3(0, 1, 0)) :
            location(_location), target(_target), up(_up) {
        }

        /// \brief Sets the viewport size and updates the aspect ratio.
        ///
        /// \param _width  Viewport width in pixels.
        /// \param _height Viewport height in pixels.
        void set_aspect(int _width, int _height) {
            width = _width;
            height = _height;
            aspect = float(width) / float(height);
        }

        /// \brief Initializes camera position, target and up vectors.
        ///
        /// \param _location Camera position in world space.
        /// \param _target   Look-at target point in world space.
        /// \param _up       Up direction vector (defaults to +Y).
        void setup(const btm::fvec3& _location,
                   const btm::fvec3& _target,
                   const btm::fvec3& _up = btm::fvec3(0, 1, 0))
        {
            location = _location;
            target = _target;
            up = _up;
        }

        void set_position(const btm::fvec3& _location) {
            location = _location;
        }
        void set_target(const btm::fvec3& _target) {
            target = _target;
        }
        void set_up(const btm::fvec3& _up) {
            up = _up;
        }


        /// \brief Sets the vertical field of view.
        ///
        /// \param fov_in_radians Field of view in radians.
        void set_fov(float fov_in_radians) {
            fov = fov_in_radians;
        }

        /// \brief Sets viewport rectangle and updates aspect ratio.
        ///
        /// \param _left   Left coordinate of the viewport within the window.
        /// \param _bottom Bottom coordinate of the viewport within the window.
        /// \param _width  Viewport width in pixels.
        /// \param _height Viewport height in pixels.
        void set_viewport(int _left, int _bottom, int _width, int _height) {
            left = _left;
            bottom = _bottom;
            width = _width;
            height = _height;
            aspect = float(width) / float(height);
        }

        /// \brief Sets the near and far clipping planes.
        ///
        /// \param nearP Distance to the near plane.
        /// \param farP  Distance to the far plane.
        void set_depth_range(float nearP, float farP) {
            nearPlane = nearP;
            farPlane = farP;
        }

        /// \brief Returns the normalized forward direction vector.
        ///
        /// Defined as the normalized vector from `location` to `target`.
        btm::fvec3 forward() const { return (target - location).normalize(); }

        /// \brief Returns the normalized right direction vector.
        ///
        /// Computed as the cross product of `forward()` and `up`.
        btm::fvec3 right()   const { return forward().cross(up).normalize(); }

        /// \brief Returns the camera up vector derived from forward and right.
        ///
        /// Computed as the cross product of `right()` and `forward()`.
        btm::fvec3 upVec()   const { return right().cross(forward()); }

        /// \brief Returns the distance between camera location and target.
        float distance() const { return (target - location).length(); }

        /// \brief Builds the perspective projection matrix for the camera.
        ///
        /// Uses the current `fov`, `aspect`, `nearPlane` and `farPlane`.
        btm::fmat4 projection_matrix() {
            return perspective_matrix(fov, aspect, nearPlane, farPlane);
        }

        /// \brief Builds the view matrix using a look-at transform.
        ///
        /// Uses the current `location`, `target` and `up`.
        btm::fmat4 view_matrix() {
            return lookAt(location, target, up);
        }

        /// \brief Returns the combined view-projection matrix.
        ///
        /// Computed as `view_matrix() * projection_matrix()`.
        btm::fmat4 perspective() {
            return view_matrix() * projection_matrix();
        }

        /// \brief Applies the currently configured viewport to OpenGL.
        ///
        /// Calls `glViewport(left, bottom, width, height)`.
        void set_viewport() {
            glViewport(left, bottom, width, height);
        }

        /// \brief Pans the camera parallel to the view plane.
        ///
        /// Translates both `location` and `target` along the right and up
        /// directions, scaled by the current distance to the target.
        ///
        /// \param dx Horizontal pan amount (screen space units).
        /// \param dy Vertical pan amount (screen space units).
        void pan(float dx, float dy)
        {
            // Distance from camera to target (the point you're orbiting around)
            float d = distance();

            // World-space height of the viewport at distance d
            float screenHeight = 2.0f * d * tanf(fov * 0.5f);

            // World units per pixel
            float worldPerPixel = screenHeight / height;

            // Compute world-space pan
            fvec3 pan =
                (-dx * worldPerPixel) * right() +
                (dy * worldPerPixel) * upVec();

            // Apply translation
            location += pan;
            target += pan;
        }

        void zoom(float delta) {
            float s = distance() * 0.001f;
            btm::fvec3 delta_vec = forward() * delta * s;
            location += delta_vec;
        }

        /// \brief Returns the current camera position in world space.
        const btm::fvec3& get_location() const { return location; }

        /// \brief Returns the current look-at target in world space.
        const btm::fvec3& get_target() const { return target; }

        const btm::fvec3& get_up() const { return up; }

        const int viewport_width() const { return width; }
        const int viewport_height() const { return height; }

        void apply(gl_shader* shdr) {
            fmat4 view = view_matrix();
            fmat4 projection = projection_matrix();
            fvec3 loc = get_location();
            shdr->set_mat4("view", view);
            shdr->set_mat4("projection", projection);
            shdr->set_vec3("cameraPos", loc);
        }

        // Computes the optimal camera distance so that an object fits fully on screen
        // fovDeg: vertical field of view in degrees
        // objWidth, objHeight: size of the model's bounding box (in world units)
        // aspect: screen aspect ratio = width / height
        //
        // Returns the required distance from camera to model center.
        float computeCameraDistance(float fovDeg, float objWidth, float objHeight) {
            if (fovDeg <= 0.0f || fovDeg >= 179.0f) {
                throw std::invalid_argument("FOV must be between 0 and 179 degrees.");
            }
            if (objWidth <= 0.0f || objHeight <= 0.0f) {
                throw std::invalid_argument("Object dimensions must be positive.");
            }
            if (aspect <= 0.0f) {
                throw std::invalid_argument("Aspect ratio must be positive.");
            }

            // Convert FOV to radians
            float fovRad = fovDeg * (3.14159265359f / 180.0f);

            // Half sizes
            float halfHeight = objHeight * 0.5f;
            float halfWidth = objWidth * 0.5f;

            // Required distance to fit height
            float distHeight = halfHeight / std::tan(fovRad * 0.5f);

            // Compute horizontal FOV from vertical FOV and aspect ratio:
            // tan(hFOV/2) = tan(vFOV/2) * aspect
            float hFovRad = 2.0f * std::atan(std::tan(fovRad * 0.5f) * aspect);

            // Required distance to fit width
            float distWidth = halfWidth / std::tan(hFovRad * 0.5f);

            // Use whichever distance is larger (guarantees the model fits)
            return std::max(distHeight, distWidth);
        }

    };
}

#endif // __camera_h__
