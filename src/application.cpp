#include <string>
#include <iostream>
#include "glew.h"

#include "window.h"
#include "timer.h"

#include "application.h"
#include "gl_context.h"

namespace btm_framework
{

    static winApplication *theApp = nullptr;
    static HINSTANCE ghInstance;
    HINSTANCE get_hInstance()
    {
        return ghInstance;
    }
    void set_hInstance(HINSTANCE hInstance)
    {
        ghInstance = hInstance;
    }

    winApplication *GetApp()
    {
        return theApp;
    }

    winApplication::winApplication()
    {
        if (theApp != nullptr)
        {
            MessageBox(nullptr, "Application instance already exists!", "Error", MB_ICONERROR);
            exit(1);
        }
        theApp = this;
    }
    winApplication::~winApplication()
    {
        if (pFrame != nullptr)
        {
            delete pFrame;
            pFrame = nullptr;
        }
    }

    FrameWindow *winApplication::getMainWindow(HINSTANCE hInstance)
    {
        if (pFrame == nullptr)
        {
            pFrame = new FrameWindow(hInstance);
        }
        return pFrame;
    }

    FrameWindow *glApplication::getMainWindow(HINSTANCE hInstance)
    {
        if (pFrame == nullptr)
        {
            pFrame = new FrameWindow(hInstance);
        }
        return pFrame;
    }

    static void RegisterFrameWindowClass(HINSTANCE hInst, LPCSTR className, UINT style, WNDPROC wndProc)
    {
        WNDCLASSEX m_wcex;
        ZeroMemory(&m_wcex, sizeof(WNDCLASSEX));
        m_wcex.cbSize = sizeof(WNDCLASSEX);
        m_wcex.style = style;
        m_wcex.lpfnWndProc = wndProc;
        m_wcex.hInstance = hInst;
        m_wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        m_wcex.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);
        m_wcex.lpszClassName = className;
        // allow the application to customize the window class (e.g. set icons, menu, etc.)
        theApp->precreate_window(hInst, &m_wcex);
        RegisterClassEx(&m_wcex);
    }

    static bool RegisterViewWindowClass(HINSTANCE hInst, LPCSTR className, UINT style, WNDPROC wndProc)
    {
        WNDCLASS wc = {0};
        wc.style = style;
        wc.lpfnWndProc = wndProc;
        wc.hInstance = hInst;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = className;
        return RegisterClass(&wc) != 0;
    }

    FrameWindow *create_main_window(bool has_view, int width, int height, LPCSTR title)
    {
        RegisterFrameWindowClass(GetModuleHandle(nullptr), "BTM_WindowClass", CS_HREDRAW | CS_VREDRAW, cWindow::StaticWndProc);
        RegisterViewWindowClass(GetModuleHandle(nullptr), "ViewWindowClass", CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS, cWindow::StaticWndProc);

        FrameWindow *pFrame = theApp->getMainWindow(GetModuleHandle(nullptr));
        if (!pFrame)
        {
            MessageBox(nullptr, "Failed to create main window", "Error", MB_ICONERROR);
            return nullptr;
        }
        pFrame->config.create_view = has_view;
        pFrame->config.width = width;
        pFrame->config.height = height;
        pFrame->config.title = title;

        DWORD windowStyle = WS_OVERLAPPEDWINDOW;     // define our window style
        DWORD windowExtendedStyle = WS_EX_APPWINDOW; // define the window's extended style

        RECT windowRect = {0, 0, width, height}; // define our window coordinates

        // adjust window, account for window borders
        AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);

        HWND hFrame = CreateWindowEx(0, "BTM_WindowClass", title,
                                     WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                     windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                                     nullptr, nullptr, GetModuleHandle(nullptr),
                                     pFrame // lpCreateParams -> 'this' for FrameWindow
        );
        pFrame->hWnd = hFrame;

        if (pFrame->hWnd)
        {
            // after the window is created and OpenGL context is ready, we can initialize GLEW and application-specific resources
            // initialize wrangler library
            glewInit();
            // initialize application-specific resources
            theApp->init_application();

            // show the window
            pFrame->show_window(SW_SHOWNORMAL);
        }
        else
        {
            delete pFrame;
            MessageBox(nullptr, "Failed to create main window", "Error", MB_ICONERROR);
            return nullptr;
        }

        return pFrame;
    }

    bool init_framework()
    {
        return true;
    }

    bool pollEvents()
    {
        bool running = true;
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                running = false;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return running;
    }

    // helper functions for rendering
    // the application can call these functions in its render() method to perform rendering using the current OpenGL context
    bool begin_render(){
        GLContext* context = theApp->get_gl_context();
        if (context) {
            context->begin_render();
            return true;
        }
        return false;        
    }
    void end_render(){
        GLContext* context = theApp->get_gl_context();
        if (context)
            context->end_render();
    }
    GLContext* get_current_gl_context(){
        return theApp->get_gl_context();
    }

} // namespace btm_framework
