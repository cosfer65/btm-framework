#include "gl_context.h"

namespace btm_framework
{
    GLContext::GLContext(HWND hwnd)
    {
        // Initialize OpenGL context here using hwnd
        PIXELFORMATDESCRIPTOR pfd = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,
            0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0,
            32, // depth buffer
            32, // stencil buffer
            0,
            PFD_MAIN_PLANE,
            0, 0, 0, 0};

        hWnd = hwnd;
        hDC = GetDC(hwnd);
        int pf = ChoosePixelFormat(hDC, &pfd);
        if (!pf){
            ReleaseDC(hwnd, hDC);
            return;
        }

        if (!SetPixelFormat(hDC, pf, &pfd)){
            ReleaseDC(hwnd, hDC);
            return;
        }
        hGLRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hGLRC);
    }

    GLContext::~GLContext()
    {
        // Clean up OpenGL context here
        if (hGLRC)
        {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(hGLRC);
            hGLRC = nullptr;
        }
        if (hDC)
        {
            ReleaseDC(hWnd, hDC);
            hDC = nullptr;
        }
    }

    void GLContext::begin_render()
    {
        // Set up OpenGL state for rendering
        wglMakeCurrent(hDC, hGLRC);
    }

    void GLContext::end_render()
    {
        // Swap buffers or perform any necessary cleanup after rendering
        SwapBuffers(hDC);
    }
}