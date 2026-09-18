#pragma once

#include "types.h"

// this module ties the ppg framework with the btm-framework used in TheMeshProject
namespace btm {
    // we hide the btm-framework behind a simple interface,
    // to avoid exposing the btm-framework to the user of the ppg-framework

    class platform {
        window_handle main_window_handle;
    public:
        platform() = default;
        window_handle create_window(int width, int height, const char* title);
        bool application_active();
    };

    draw_surface get_current_draw_surface();
    int get_draw_surface_width(draw_surface surface);
    int get_draw_surface_height(draw_surface surface);
    bool begin_render(draw_surface surface);
    void end_render(draw_surface surface);

}
