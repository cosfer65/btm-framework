#pragma once

#include <windows.h>
#include <string>

typedef void (*mouse_callback)(int x, int y, unsigned __int64 extra);
typedef void (*kbd_callback)(int keycode);
typedef int (*cmd_callback)(int keycode);



namespace btm
{
    class cWindow;
    class FrameWindow;
    class GLContext;

    class application
    {
        FrameWindow *pFrame = nullptr;
        bool is_minimized = false;
        std::string m_app_name = "TheMeshProject";

        mouse_callback mouse_move_callback = nullptr;
        mouse_callback lmouse_down_callback = nullptr;
        mouse_callback lmouse_up_callback = nullptr;
        mouse_callback lmouse_dblclick_callback = nullptr;
        mouse_callback rmouse_down_callback = nullptr;
        mouse_callback rmouse_up_callback = nullptr;
        mouse_callback rmouse_dblclick_callback = nullptr;
        mouse_callback mmouse_down_callback = nullptr;
        mouse_callback mmouse_up_callback = nullptr;
        mouse_callback m_dblclick_callback = nullptr;
        mouse_callback mouse_wheel_callback = nullptr;
        kbd_callback key_down_callback = nullptr;
        kbd_callback key_up_callback = nullptr;
        cmd_callback command_callback = nullptr;
    public:
        application();
        virtual ~application();

        void set_app_name(const std::string &name) { m_app_name = name; }
        const std::string& get_app_name() const { return m_app_name; }

        virtual void precreate_window(HINSTANCE hInstance, WNDCLASSEX *m_wcex) {}
        virtual FrameWindow *getMainWindow(HINSTANCE hInstance);
        virtual cWindow *get_active_view();
        virtual GLContext *get_gl_context();
        virtual void init_application() {}
        virtual void terminate() {}

        virtual void step_simulation(float fElapsed) {}
        virtual void pause_simulation(float fElapsed) {}
        virtual void resume_simulation(float fElapsed) {}
        virtual void restart_simulation() {}
        virtual void render();

        virtual int onCommand(int cmd) { if (command_callback) return command_callback(cmd); return 0; }
        virtual void onKeyDown(int keycode) { if (key_down_callback) key_down_callback(keycode); }
        virtual void onKeyUp(int keycode) { if (key_up_callback) key_up_callback(keycode); }
        virtual void onMouseMove(int x, int y, unsigned __int64 extra) { if (mouse_move_callback) mouse_move_callback(x, y, extra); }
        virtual void onLMouseDown(int x, int y, unsigned __int64 extra) { if (lmouse_down_callback) lmouse_down_callback(x, y, extra); }
        virtual void onLMouseUp(int x, int y, unsigned __int64 extra) { if (lmouse_up_callback) lmouse_up_callback(x, y, extra); }
        virtual void onLDblClick(int x, int y, unsigned __int64 extra) { if (lmouse_dblclick_callback) lmouse_dblclick_callback(x, y, extra); }
        virtual void onRMouseDown(int x, int y, unsigned __int64 extra) { if (rmouse_down_callback) rmouse_down_callback(x, y, extra); }
        virtual void onRMouseUp(int x, int y, unsigned __int64 extra) { if (rmouse_up_callback) rmouse_up_callback(x, y, extra); }
        virtual void onRDblClick(int x, int y, unsigned __int64 extra) { if (rmouse_dblclick_callback) rmouse_dblclick_callback(x, y, extra); }
        virtual void onMMouseDown(int x, int y, unsigned __int64 extra) { if (mmouse_down_callback) mmouse_down_callback(x, y, extra); }
        virtual void onMMouseUp(int x, int y, unsigned __int64 extra) { if (mmouse_up_callback) mmouse_up_callback(x, y, extra); }
        virtual void onMDblClick(int x, int y, unsigned __int64 extra) { if (m_dblclick_callback) m_dblclick_callback(x, y, extra); }
        virtual void onMouseWheel(int delta, unsigned __int64 extra_btn) { if (mouse_wheel_callback) mouse_wheel_callback(delta, 0, extra_btn); }
        virtual void windowMinimized(bool minimized) { is_minimized = minimized; }
        virtual void windowMaximized() { is_minimized = false; }

        void set_mouse_move_callback(mouse_callback cb) { mouse_move_callback = cb; }
        void set_lmouse_down_callback(mouse_callback cb) { lmouse_down_callback = cb; }
        void set_lmouse_up_callback(mouse_callback cb) { lmouse_up_callback = cb; }
        void set_lmouse_dblclick_callback(mouse_callback cb) { lmouse_dblclick_callback = cb; }
        void set_rmouse_down_callback(mouse_callback cb) { rmouse_down_callback = cb; }
        void set_rmouse_up_callback(mouse_callback cb) { rmouse_up_callback = cb; }
        void set_rmouse_dblclick_callback(mouse_callback cb) { rmouse_dblclick_callback = cb; }
        void set_mmouse_down_callback(mouse_callback cb) { mmouse_down_callback = cb; }
        void set_mmouse_up_callback(mouse_callback cb) { mmouse_up_callback = cb; }
        void set_m_dblclick_callback(mouse_callback cb) { m_dblclick_callback = cb; }
        void set_mouse_wheel_callback(mouse_callback cb) { mouse_wheel_callback = cb; }
        void set_key_down_callback(kbd_callback cb) { key_down_callback = cb; }
        void set_key_up_callback(kbd_callback cb) { key_up_callback = cb; }
        void set_command_callback(cmd_callback cb) { command_callback = cb; }

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

} // namespace btm
