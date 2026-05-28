#include "application.h"
#include "timer.h"
#include "glew.h"

#include "utils.h"

using namespace btm;

class tut_01_app : public application
{
public:
    // constructor and destructor
    tut_01_app() : application() {
        // print a message to the debug output to indicate that the constructor is called
        odprintf("tut_01_app constructor called\n");
    }
    virtual ~tut_01_app() {
        // print a message to the debug output to indicate that the destructor is called
        odprintf("tut_01_app destructor called\n");
    }

    // override necessary methods from application
    // these methods will be called by the framework at appropriate times during the application lifecycle

    // This method is called during application initialization, 
    // after the main window is created and before the main loop starts
    // OpenGL context is available at this point, so you can perform any necessary OpenGL setup here
    virtual void init_application() {
        // print a message to the debug output to indicate that the init_application method is called
        odprintf("tut_01_app init_application called\n");
    }

    // This method is called when the application is terminating,
    // you can perform any necessary cleanup here
    virtual void terminate() {
        // print a message to the debug output to indicate that the terminate method is called
        odprintf("tut_01_app terminate called\n");
    }
    
    // This method is called every frame with the elapsed time since the last frame,
    // you can update your simulation state based on the elapsed time here
    virtual void step_simulation(float fElapsed) {
        // print a message to the debug output to indicate that the step_simulation method is called with the elapsed time
        odprintf("tut_01_app step_simulation called with elapsed time: %f seconds\n", fElapsed);
    }

    // Override the render method to perform our custom rendering
    // This method is called every frame after step_simulation, and is where you should perform all your rendering using OpenGL
    // In this example, we simply clear the screen with a solid color, but you can replace this with your actual rendering code
    void render() override
    {
        // begin rendering using the current OpenGL context
        begin_render();

        // Set the clear color to black and clear the color and depth buffers
        glClearColor(0.2f, 0.4f, 0.6f, 1.f);

        // clear screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // end rendering and present the frame
        end_render();
    }
};

// Windows application entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
    // the application instance
    // we are creating a global instance of our application class, which will be used throughout the program
    // this is a GL application, so we create an instance of tut_01_app, which inherits from application
    // and overrides necessary methods for OpenGL rendering
    tut_01_app the_app;
    the_app.set_app_name("Tutorial 01 - Basic OpenGL Window");

    the_app.init_and_run();
    return 0;
}
