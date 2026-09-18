#include "application.h"
#include "window.h"
#include "gl_context.h"

#include "platform.h"

namespace btm {
    bool platform::application_active() {
        return btm::pollEvents();
    }
    window_handle platform::create_window(int width, int height, const char* title) {
        btm::FrameWindow* window = btm::create_main_window(false, width, height, title);
        main_window_handle = static_cast<window_handle>(window);
        return main_window_handle;
    }
    draw_surface get_current_draw_surface() {
        return btm::get_current_gl_context();
    }

    int get_draw_surface_width(draw_surface surface) {
        btm::GLContext* context = static_cast<btm::GLContext*>(surface);
        return context ? context->width() : 0;
    }
    int get_draw_surface_height(draw_surface surface) {
        btm::GLContext* context = static_cast<btm::GLContext*>(surface);
        return context ? context->height() : 0;
    }

    bool begin_render(draw_surface surface) {
        btm::GLContext* context = static_cast<btm::GLContext*>(surface);
        if (context)
        {
            context->begin_render();
            return true;
        }
        return false;
    }
    void end_render(draw_surface surface) {
        btm::GLContext* context = static_cast<btm::GLContext*>(surface);
        if (context)
            btm::end_render();
    }
}

#if 1
// def _REQUIRED_PLATFORM_MAIN
// declaration of the main function, which is defined in the main.cpp file
int my_main();

// entry point of the application, which is called by the operating system
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow) {
    // initialize the btm-framework, which is the backbone of the ppg-framework, and is used in TheMeshProject
    btm::application the_app;
    btm::init_framework();

    return my_main();
}
#endif
