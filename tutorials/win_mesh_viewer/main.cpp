#include "application.h"
#include "timer.h"
#include "glew.h"
#include "dynamic_menu.h"
#include "gl_context.h"

#include "mesh_explicit.h"
#include "mesh_renderer.h"

// we are using the btm_framework namespace for convenience, 
// but you can also use the full namespace when calling functions and classes.
// using namespace btm_framework;

void create_application_menu(btm_framework::FrameWindow* pFrame, btm_framework::Menu& menu)
{
    // Create top-level menus
    HMENU fileMenu = menu.create_submenu("File");

    // Add items dynamically
    // menu.add_item(fileMenu, "Open...", [] {
    //     // open file dialog
    //     });

    int exitId = menu.add_item(fileMenu, "Exit", [pFrame]() {
        // quit, we can also post a message to the main window to trigger the close event
        PostMessage(pFrame->hWnd, WM_CLOSE, 0, 0);
        });

    // Attach to window
    menu.attach_to_window(pFrame->hWnd);
}

// This function creates a simple mesh with 4 vertices and 2 triangles for demonstration purposes.
void create_mesh(btm_framework::MeshExplicit<float>& m) {
    // first create vertices
    std::uint32_t i1 = m.add_vertex({ -1.f, -0.5f, 0.f });
    std::uint32_t i2 = m.add_vertex({ -.33f, 0.5f, 0.f });
    std::uint32_t i3 = m.add_vertex({ .33f, -.5f, 0.f });
    std::uint32_t i4 = m.add_vertex({ 1.f, 0.5f, 0.f });
    // then create triangles using the vertex indices
    m.add_triangle(i1, i3, i2);
    m.add_triangle(i3, i4, i2);
    // finally build adjacency information for the mesh, 
    // which is used for various mesh processing algorithms and rendering techniques.
    m.build_adjacency();
}

void render(const btm_framework::MeshRenderer<float>& renderer)
{
    btm_framework::GLContext* context = btm_framework::get_current_gl_context();
    if (!context)
        return;

    int width = context->width();
    int height = context->height();
    if (height <= 0)
        height = 1;

    glClearColor(0.2f, 0.4f, 0.6f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);

    renderer.render();

    btm_framework::end_render();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    btm_framework::glApplication the_app;

    btm_framework::init_framework();

    btm_framework::FrameWindow* pFrame = btm_framework::create_main_window(false, 800, 450, "Simple Mesh Viewer");
    btm_framework::Menu menu;
    create_application_menu(pFrame, menu);
    
    // here we can create/load our mesh
    btm_framework::MeshExplicit<float> mesh;
    create_mesh(mesh);
    // and then create a renderer for the mesh, which will be used in the render loop to draw the mesh on the screen.
    btm_framework::MeshRenderer<float> renderer(mesh);

    btm_framework::start_timer();
    btm_framework::get_elapsed_time();

    while (btm_framework::pollEvents())
    {
        float fElapsed = (float)btm_framework::get_elapsed_time();
        // Here we can update our application state based on the elapsed time, user input, or other factors.

        // and we conclude with the screen update by calling the render function, 
        // which will use the MeshRenderer to draw the mesh on the screen.
        render(renderer);

        {
            static int m_nFrames = 0;
            static float tot = 0;
            tot += fElapsed;
            m_nFrames++;
            if (tot >= 1.f)
            {
                char txt[200];
                sprintf_s(txt, "Simple Mesh Viewer, fps:%d", m_nFrames);
                HWND hWnd = pFrame->hWnd;
                SetWindowText(hWnd, txt);
                tot = 0;
                m_nFrames = 0;
            }
        }
    }
    the_app.terminate();
    btm_framework::stop_timer();

    return 0;
}