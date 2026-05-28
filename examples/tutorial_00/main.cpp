#include "application.h"
#include "glew.h"

// This is a minimal example of a Windows application using the btm. 
// It creates a window and renders a simple colored background.
class win_minimal_example : public btm::application
{
public:
    win_minimal_example() : application() {}
    virtual ~win_minimal_example() {}

    // Override the render method to perform our custom rendering
    void render() override {
        // This function can be used to render the current state of the simulation
        // For example, you could clear the screen, draw objects, etc.
        btm::begin_render();

        // Set the clear color to black and clear the color and depth buffers
        glClearColor(0.2f, 0.4f, 0.6f, 1.f);

        // clear screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // we finish the rendering by calling end_render(), which swaps the buffers to display the rendered frame
        // and performs any necessary cleanup after rendering
        btm::end_render();
    }
};

// Windows application entry point
// The WinMain function is the entry point for a Windows application. It initializes the application and starts the main loop.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    // create an instance of our application class, which will be used throughout the program
    win_minimal_example app;
    app.set_app_name("Minimal Example - Basic OpenGL Window");
    // initialization of the framework: btm::init_framework();
    // is done inside the init_and_run() method of the application class, which is called below. 
    // This method will set up the application window and start the main loop.
    app.init_and_run();
    return 0;
}
