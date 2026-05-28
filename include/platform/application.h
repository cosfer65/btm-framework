#pragma once

#include <windows.h>
#include <string>

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

        virtual int onCommand(int cmd) { return 0; }
        virtual void onKeyDown(int keycode) {}
        virtual void onKeyUp(int keycode) {}
        virtual void onMouseMove(int x, int y, unsigned __int64 extra) {}
        virtual void onLMouseDown(int x, int y, unsigned __int64 extra) {}
        virtual void onLMouseUp(int x, int y, unsigned __int64 extra) {}
        virtual void onLDblClick(int x, int y, unsigned __int64 extra) {}
        virtual void onRMouseDown(int x, int y, unsigned __int64 extra) {}
        virtual void onRMouseUp(int x, int y, unsigned __int64 extra) {}
        virtual void onRDblClick(int x, int y, unsigned __int64 extra) {}
        virtual void onMMouseDown(int x, int y, unsigned __int64 extra) {}
        virtual void onMMouseUp(int x, int y, unsigned __int64 extra) {}
        virtual void onMDblClick(int x, int y, unsigned __int64 extra) {}
        virtual void onMouseWheel(int delta, unsigned __int64 extra_btn) {}
        virtual void windowMinimized(bool minimized) { is_minimized = minimized; }
        virtual void windowMaximized() { is_minimized = false; }

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
