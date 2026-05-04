#include <string>
#include <iostream>

#include "window.h"
#include "btm_core.h"
#include "timer.h"

#include "application.h"

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
            pFrame = new glFrameWindow(hInstance);
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

    FrameWindow *create_main_window(HINSTANCE hInstance, LPCSTR className, LPCSTR title)
    {
        FrameWindow *pFrame = theApp->getMainWindow(hInstance);
        if (!pFrame)
        {
            MessageBox(nullptr, "Failed to create main window", "Error", MB_ICONERROR);
            return nullptr;
        }

        DWORD windowStyle = WS_OVERLAPPEDWINDOW;     // define our window style
        DWORD windowExtendedStyle = WS_EX_APPWINDOW; // define the window's extended style

        RECT windowRect = {0, 0, 800, 600}; // define our window coordinates

        // adjust window, account for window borders
        AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);

        HWND hFrame = CreateWindowEx(0, className, title,
                                     WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                     windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                                     nullptr, nullptr, hInstance,
                                     pFrame // lpCreateParams -> 'this' for FrameWindow
        );

        // after the window is created and OpenGL context is ready, we can initialize GLEW and application-specific resources
        // initialize wrangler library 
        glewInit();
        // initialize application-specific resources
        theApp->init_application();

        return pFrame ? pFrame : nullptr;
    }

    bool init_framework()
    {
        RegisterFrameWindowClass(GetModuleHandle(nullptr), "BTM_WindowClass", CS_HREDRAW | CS_VREDRAW, cWindow::StaticWndProc);
        RegisterViewWindowClass(GetModuleHandle(nullptr), "ViewWindowClass", CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS, cWindow::StaticWndProc);
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

} // namespace btm_framework
