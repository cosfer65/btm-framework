#pragma once

#include "utils.h" // force link of library
#include "window.h"

namespace btm_framework
{
    class winApplication
    {
    public:
        FrameWindow *pFrame = nullptr;
        winApplication();
        ~winApplication();

        bool is_minimized = false;

        virtual void precreate_window(HINSTANCE hInstance, WNDCLASSEX *m_wcex) {}
        virtual FrameWindow *getMainWindow(HINSTANCE hInstance);
        virtual cWindow *get_active_view()
        {
            if (pFrame)
                return pFrame->get_view();
            return nullptr;
        }
        virtual void init_application() {}
        virtual void terminate() {}

        virtual void step_simulation(float fElapsed) {}
        virtual void pause_simulation(float fElapsed) {}
        virtual void resume_simulation(float fElapsed) {}
        virtual void restart_simulation() {}
        virtual void render() {}

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
    };

    class glApplication : public winApplication
    {
    public:
        glApplication() : winApplication() {}
        ~glApplication() {}
        virtual FrameWindow *getMainWindow(HINSTANCE hInstance) override;
        virtual void render()
        {
            get_active_view()->render();
        }
    };

    winApplication *GetApp();
    HINSTANCE get_hInstance();
    void set_hInstance(HINSTANCE hInstance);
    bool init_framework();
    FrameWindow *create_main_window(HINSTANCE hInstance, LPCSTR className, LPCSTR title);
    bool run_application();
    bool pollEvents();
} // namespace btm_framework
