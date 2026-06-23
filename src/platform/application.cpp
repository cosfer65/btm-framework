#include <string>
#include <iostream>
#include "glew.h"

#include "window.h"
#include "timer.h"

#include "application.h"
#include "gl_context.h"

namespace btm
{
    command_map_t command_target::command_map;
    callback_map_t callback_registry::callback_map;

    static application *theApp = nullptr;
    static HINSTANCE ghInstance;
    HINSTANCE get_hInstance()
    {
        return ghInstance;
    }
    void set_hInstance(HINSTANCE hInstance)
    {
        ghInstance = hInstance;
    }

    application *GetApp()
    {
        return theApp;
    }

    application::application()
    {
        if (theApp != nullptr)
        {
            MessageBox(nullptr, "Application instance already exists!", "Error", MB_ICONERROR);
            exit(1);
        }
        theApp = this;
    }
    application::~application()
    {
        if (pFrame != nullptr)
        {
            delete pFrame;
            pFrame = nullptr;
        }
    }
    FrameWindow* application::create_main_window(HINSTANCE hInstance) {
        return new FrameWindow(hInstance);
    }
    void application::set_main_window(FrameWindow* frame) {
        if (pFrame != nullptr) {
            delete pFrame;
        }
        pFrame = frame;
    }
    FrameWindow *application::get_main_window(HINSTANCE hInstance)
    {
        if (pFrame == nullptr)
        {
            pFrame = create_main_window(hInstance);
        }
        return pFrame;
    }

    cWindow *application::get_active_view()
    {
        if (pFrame)
            return pFrame->get_view();
        return nullptr;
    }

    GLContext *application::get_gl_context()
    {
        if (pFrame)
            return pFrame->get_gl_context();
        return nullptr;
    }
    
    void application::render()
    {
        if (get_active_view())
            get_active_view()->render();
    }

    int application::init_and_run()
    {
        // initialize the framework
        init_framework();

        // create the main application window and show it
        // we use the create_main_window function to create a FrameWindow instance, which will be our main application window
        // we pass the instance handle, and window title to the function
        FrameWindow *pFrame = btm::create_main_window(true, 600, 400, m_app_name.c_str());

        // run the application main loop
        // start the timer before we enter the main loop
        start_timer();
        // call once to initialize elapsed time
        get_elapsed_time();

        // Main message loop
        // pollEvents() is a function that processes Windows messages (like keyboard and mouse input, window events, etc.)
        // and returns true as long as the application should continue running
        while (pollEvents())
        {
            // Idle time → render a frame
            // calculate elapsed time since last frame
            float fElapsed = (float)get_elapsed_time();
            // update the simulation state based on the elapsed time
            step_simulation(fElapsed);
            // render the scene using the application instance's render method
            render();

            // frame counting mechanism
            {
                static int m_nFrames = 0; // frame Counter
                static float tot = 0;     // time couner
                tot += fElapsed;          // increment counter
                m_nFrames++;
                if (tot >= 1.f) // one second reached
                {
                    char txt[200];
                    sprintf_s(txt, "%s, fps:%d", m_app_name.c_str(), m_nFrames);
                    HWND hWnd = pFrame->hWnd;
                    // update the window title with the current frames per second (FPS) count
                    // SetWindowText is a Windows API function that sets the text of a window's title bar
                    SetWindowText(hWnd, txt);
                    tot = 0; // reset counters
                    m_nFrames = 0;
                }
            }
        }
        // clean up application-specific resources
        terminate();
        // stop the timer
        stop_timer();
        return 0;
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

        FrameWindow *pFrame = theApp->create_main_window(GetModuleHandle(nullptr));
        if (!pFrame)
        {
            MessageBox(nullptr, "Failed to create main window", "Error", MB_ICONERROR);
            return nullptr;
        }
        theApp->set_main_window(pFrame);
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
    bool begin_render()
    {
        GLContext *context = theApp->get_gl_context();
        if (context)
        {
            context->begin_render();
            return true;
        }
        return false;
    }
    void end_render()
    {
        GLContext *context = theApp->get_gl_context();
        if (context)
            context->end_render();
    }
    GLContext *get_current_gl_context()
    {
        return theApp->get_gl_context();
    }

    void SetAppIcon(int resource_id) {
        HWND hwnd = theApp->get_main_window(get_hInstance())->hWnd;
        HINSTANCE hInst = nullptr;// ghInstance;
        // Load a 64×64 icon for big icon (task switcher)
        HICON hIconLarge = (HICON)LoadImage(
            hInst,
            MAKEINTRESOURCE(resource_id),   // icon ID from .rc file
            IMAGE_ICON,
            64, 64,
            LR_DEFAULTCOLOR
        );

        // Load a 32×32 icon for small (title bar)
        HICON hIconSmall = (HICON)LoadImage(
            hInst,
            MAKEINTRESOURCE(resource_id),
            IMAGE_ICON,
            32, 32,
            LR_DEFAULTCOLOR
        );

        // Apply icons
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconLarge);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
    }


} // namespace btm
