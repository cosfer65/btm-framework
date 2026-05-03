#include "application.h"
#include "timer.h"

using namespace btm_framework;

bool run_application(glApplication* the_app){
    // start the timer before we enter the main loop
    get_global_timer()->start();
    // call once to initialize elapsed time
    get_global_timer()->get_elapsed_time();

    MSG msg;
    // Main message loop
    while (pollEvents()) {
        // Idle time → render a frame
        // calculate elapsed time since last frame
        float fElapsed = (float)get_global_timer()->get_elapsed_time();
        // step the simulation and render
        the_app->step_simulation(fElapsed);
        the_app->render();
    }
    // clean up application-specific resources
    the_app->terminate();
    // stop the timer
    get_global_timer()->stop();

    return true;
}

// Windows application entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow){
    // the application instance 
    glApplication the_app;

    // initialize the framework
    init();

    // create the main application window and show it
    FrameWindow* pFrame = create_main_window(GetModuleHandle(nullptr), "BTM_WindowClass", "TheMeshProject");
    pFrame->show_window(SW_SHOWNORMAL);

    // run the application main loop
    run_application(&the_app);

    return 0;
}