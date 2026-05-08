#pragma once

#include <windows.h>
#include <string>

namespace btm_framework
{
    class cWindow
    {
        void handle_mouse_message(UINT message, WPARAM wParam, LPARAM lParam);
        int wwidth = 0, wheight = 0;

    public:
        struct Config
        {
            int width = 1280;
            int height = 720;
            std::string title = "TheMeshProject";
        };

        HWND hWnd = nullptr;
        cWindow() {};
        virtual ~cWindow() {}
        // Static window procedure
        static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        // Instance window procedure
        virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

        bool show_window(int nCmdShow)
        {
            if (hWnd)
            {
                ShowWindow(hWnd, nCmdShow);
                UpdateWindow(hWnd);
                return true;
            }
            return false;
        }

        virtual LRESULT OnPaint()
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);

            return 0;
        }
        virtual LRESULT OnSize(int cx, int cy)
        {
            wwidth = cx;
            wheight = cy;
            return 0;
        }
        int get_width() const { return wwidth; }
        int get_height() const { return wheight; }
        virtual LRESULT OnCreate() { return 0; }
        virtual LRESULT OnDestroy()
        {
            hWnd = nullptr;
            return 0;
        }
        virtual LRESULT OnMinimize(int wid, int hei) { 
            OnSize(wid, hei);
            return 0; 
        }
        virtual LRESULT OnMinimized(int wid, int hei) { 
            OnSize(wid, hei);
            return 0; 
        }
        virtual LRESULT OnRestored(int wid, int hei) {
            OnSize(wid, hei);
            return 0;
        }
        virtual int onCommand(int cmd) { return 0; }

        virtual void render() {}

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
        virtual void onMouseWheel(int delta, unsigned __int64 extra) {}
        virtual void onKeyDown(int key) {}
        virtual void onKeyUp(int key) {}
    };

    class GLContext;
    class glView : public cWindow
    {
        GLContext *pGLContext = nullptr;

    public:
        void begin_render();
        void end_render();

        glView();
        virtual ~glView() {}
        GLContext* get_gl_context(){
            return pGLContext;
        }

        virtual LRESULT OnCreate();
        virtual LRESULT OnPaint();
        virtual LRESULT OnDestroy();

        virtual void render();
    };

    class FrameWindow : public cWindow
    {
        GLContext* pGLContext = nullptr;
    public:
        struct Config
        {
            bool create_view = true;
            int width = 1280;
            int height = 720;
            std::string title = "TheMeshProject";
        } config;
        HINSTANCE hInst = nullptr;
        glView *pView = nullptr;

        FrameWindow(HINSTANCE hInstance);

        virtual LRESULT OnCreate();
        virtual glView *get_view() {
            if (pView == nullptr) {
                pView = new glView();
            }
            return pView;
        }

        GLContext* get_gl_context() {
            if (pView)
                return pView->get_gl_context();
            else
                return pGLContext;
        }

        virtual LRESULT OnSize(int cx, int cy);
        LRESULT OnDestroy();
    };
} // namespace btm_framework
