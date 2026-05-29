#include "application.h"
#include "timer.h"
#include "glew.h"
#include "window.h"
#include "dynamic_menu.h"
#include "gl_context.h"
#include "camera.h"
#include "shaders.h"
#include "light.h"

#include "mesh_explicit.h"
#include "mesh_renderer.h"

#include "common_dialogs.h"

#include "model.h"
#include "prim.h"
#include "arcball.h"

// we are using the btm namespace for convenience, 
// but you can also use the full namespace when calling functions and classes.
// using namespace btm;

static std::unique_ptr<btm::gl_camera> g_cam;     ///< gl_camera used to view the scene and compute view/projection.
static std::unique_ptr<btm::gl_shader> g_shader;  ///< Shader program used for mesh and helper rendering.
static std::unique_ptr<btm::gl_light> g_light;    ///< Primary scene light affecting shading.
static bool g_initialized = false;                    ///< Flag to track if application resources have been initialized.

static std::unique_ptr<cModel> m_model; ///< The currently loaded 3D model, managed as a unique pointer for automatic cleanup.
static std::vector<std::unique_ptr<gl_prim>> m_draw_parts;
static std::unique_ptr<arcball> m_arcball;                                 ///< Arcball for mouse interaction


static void reset_view() {
    if (g_cam) {
        g_cam->set_position(btm::fvec3(0, 0, 50));
        g_cam->set_target(btm::fvec3(0, 0, 0));
        g_cam->set_up(btm::fvec3(0, 1, 0));
        g_cam->set_fov(btm::dtr(45.f));
    }
    if (m_arcball) {
        m_arcball->reset();
    }
}

static void create_model_view() {
    m_draw_parts.clear();

    if (m_model) {
        const auto& parts = m_model->m_parts;
        m_draw_parts.reserve(parts.size());

        for (btm::mesh<double>* part : parts) {
            part->computeFaceProperties();
            mesh_data md;
            collect_mesh_data(part, md);
            auto prim = std::make_unique<gl_prim>();
            prim->create_from_mesh(&md, GL_FILL);
            m_draw_parts.push_back(std::move(prim));
        }
    }
}

static bool load_model(const std::string& fnm) {
    m_model.reset(load_mesh_model(fnm));
    create_model_view();
    reset_view();
    return m_model != nullptr;
}

static void create_application_menu(btm::FrameWindow* pFrame, btm::Menu& menu)
{
    // Create top-level menus
    HMENU fileMenu = menu.create_submenu("File");

    // Add items dynamically
    // menu.add_item(fileMenu, "Open...", [] {
    //     // open file dialog
    //     });

    int openId = menu.add_item(fileMenu, "Open...", [pFrame]() {
        // open file dialog
        const char* fnm = OpenFileDialog("All Files\0*.*\0\0");
        if (fnm) {
            load_model(fnm);
        }
        });

    int exitId = menu.add_item(fileMenu, "Exit", [pFrame]() {
        // quit, we can also post a message to the main window to trigger the close event
        PostMessage(pFrame->hWnd, WM_CLOSE, 0, 0);
        });

    // Attach to window
    menu.attach_to_window(pFrame->hWnd);
}

static void set_callbacks(btm::application& app) {
    // Here we can set up various callbacks for user input and commands, 
    // such as mouse movements, button clicks, keyboard input, and menu commands.

    // zoom and pan callbacks for the camera, and arcball dragging for rotation
    app.set_mouse_move_callback([](int x, int y, unsigned __int64 extra) {
        if (m_arcball) m_arcball->drag(float(x), float(y));
        if (g_cam) g_cam->mouse_move(x, y);
        });
    app.set_lmouse_down_callback([](int x, int y, unsigned __int64 extra) {
        if (m_arcball) m_arcball->beginDrag(float(x), float(y));
        });
    app.set_lmouse_up_callback([](int x, int y, unsigned __int64 extra) {
        if (m_arcball) m_arcball->endDrag();
        });

    app.set_rmouse_down_callback([](int x, int y, unsigned __int64 extra) {
        if (g_cam) g_cam->begin_drag(x, y);
        });
    app.set_rmouse_up_callback([](int x, int y, unsigned __int64 extra) {
        if (g_cam) g_cam->end_drag();
        });

    app.set_mouse_wheel_callback([](int delta, int ignore, unsigned __int64 extra) {
        if (g_cam) g_cam->zoom(float(delta));
        });
}

static void render()
{
    btm::GLContext* context = btm::get_current_gl_context();
    if (!context)
        return;

    btm::begin_render();

    int width = context->width();
    int height = context->height();
    if (height <= 0)
        height = 1;

    if (!g_initialized) {
        m_arcball.reset(new arcball(width, height));
        // Initialize application resources (camera, shader, light) on the first render call
        g_cam.reset(new btm::gl_camera(btm::fvec3(0, 0, 50), btm::fvec3(0, 0, 0), btm::fvec3(0, 1, 0)));
        g_cam->set_fov(btm::dtr(45.f));

        g_shader.reset(new gl_shader);
        g_shader->add_file(GL_VERTEX_SHADER, "resources/shaders/mesh_tools_VertexShader.glsl");
        g_shader->add_file(GL_FRAGMENT_SHADER, "resources/shaders/mesh_tools_FragmentShader.glsl");
        g_shader->load();

        g_light.reset(new gl_light(gl_light::SPOTLIGHT));
        g_light->set_position(fvec3(-20, 20, 20));
        g_light->set_ambient(fvec3(0.75f));
        g_light->set_diffuse(fvec3(0.5f));
        g_light->set_specular(fvec3(0.1f));

        // OpenGL initialization
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_MULTISAMPLE);

        g_initialized = true;
    }
    g_cam->set_aspect(width, height);
    g_cam->set_viewport();

    glClearColor(0.2f, 0.4f, 0.6f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fmat4 cam_matrix = g_cam->perspective();
    fmat4 rot_mat(m_arcball->rotation());
    // now it is safe to call resize on the arcball to update its internal viewport size, which is used for mouse coordinate normalization
    m_arcball->resize((float)width, (float)height);

    g_shader->use();
    g_light->apply(g_shader.get());
    // set the combined view matrix
    g_shader->set_mat4("camera", cam_matrix);

    // render the mesh parts with the current rotation applied
    if (m_draw_parts.size() > 0)
    {
        // render filled polygons first
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_DEPTH_TEST);
        for (const auto& part : m_draw_parts) {
            part->set_draw_mode(GL_FILL);
            part->force_black = false;
            part->view_matrix = rot_mat;   // apply the current arcball rotation to the mesh parts
            part->set_use_vertex_color(0); // ensure vertex color is disabled by default
            part->render(g_shader.get());
        }

        // then render wireframe on top
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        glPolygonOffset(-1.0f, -1.0f); // pull lines toward camera
        for (const auto& part : m_draw_parts) {
            part->set_draw_mode(GL_LINE);
            part->force_black = true; // render wireframe in black
            part->set_use_vertex_color(0); // ensure vertex color is disabled for wireframe
            part->render(g_shader.get());
        }
    }

    btm::end_render();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    btm::application the_app;

    btm::init_framework();

    btm::FrameWindow* pFrame = btm::create_main_window(false, 800, 450, "Simple Mesh Viewer");
    btm::Menu menu;
    create_application_menu(pFrame, menu);
    set_callbacks(the_app);
    
    btm::start_timer();
    btm::get_elapsed_time();

    while (btm::pollEvents())
    {
        float fElapsed = (float)btm::get_elapsed_time();
        // Here we can update our application state based on the elapsed time, user input, or other factors.

        // and we conclude with the screen update by calling the render function, 
        // which will use the MeshRenderer to draw the mesh on the screen.
        render();

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
    btm::stop_timer();

    return 0;
}
