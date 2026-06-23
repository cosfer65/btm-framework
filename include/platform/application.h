#pragma once

#include <windows.h>
#include <string>

#include "cmd_target.h"


namespace btm
{
    class cWindow;
    class FrameWindow;
    class GLContext;

    class application : public command_target
    {
        FrameWindow *pFrame = nullptr;
        bool is_minimized = false;
        std::string m_app_name = "TheMeshProject";

        kbd_callback key_down_callback = nullptr;
        kbd_callback key_up_callback = nullptr;
    public:
        application();
        virtual ~application();

        void set_app_name(const std::string &name) { m_app_name = name; }
        const std::string& get_app_name() const { return m_app_name; }

        virtual void precreate_window(HINSTANCE hInstance, WNDCLASSEX *m_wcex) {}
        virtual FrameWindow* create_main_window(HINSTANCE hInstance);
        void set_main_window(FrameWindow* frame);
        virtual FrameWindow *get_main_window(HINSTANCE hInstance);
        virtual cWindow *get_active_view();
        virtual GLContext *get_gl_context();
        virtual void init_application() {}
        virtual void terminate() {}

        virtual void step_simulation(float fElapsed) {}
        virtual void pause_simulation(float fElapsed) {}
        virtual void resume_simulation(float fElapsed) {}
        virtual void restart_simulation() {}
        virtual void render();

        virtual void on_key_down(int keycode) { if (key_down_callback) key_down_callback(WM_KEYDOWN, keycode); }
        virtual void on_key_up(int keycode) { if (key_up_callback) key_up_callback(WM_KEYUP, keycode); }

        void set_key_down_callback(kbd_callback cb) { key_down_callback = cb; }
        void set_key_up_callback(kbd_callback cb) { key_up_callback = cb; }

        virtual void exit_application() {}
        virtual int init_and_run();
    };

    application *GetApp();
    HINSTANCE get_hInstance();
    void set_hInstance(HINSTANCE hInstance);
    bool init_framework();
    FrameWindow *create_main_window(bool has_view, int width, int height, LPCSTR title);
    bool run_application();
    bool pollEvents();

    // use default window drawing
    bool begin_render();
    void end_render();
    GLContext* get_current_gl_context();
    void SetAppIcon(int resource_id);

} // namespace btm
