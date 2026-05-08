#ifndef __gl_context_h__
#define __gl_context_h__

#include <windows.h>

namespace btm_framework
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



#endif //__gl_context_h__
