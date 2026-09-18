#pragma once

#include <windows.h>

namespace btm
{
    class GLContext
    {
        HWND hWnd = nullptr;
        HGLRC hGLRC = nullptr;
        HDC hDC = nullptr;
        int m_width;
        int m_height;
    public:
        GLContext(HWND hwnd);
        ~GLContext();

        void begin_render();
        void end_render();

        int& width() { return m_width; }
        int& height() { return m_height; }
    };
}
