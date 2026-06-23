#define NOMINMAX
#include <windows.h>
#include <chrono>
#include "timer.h"


namespace btm {

    class btm_timer {
        std::chrono::high_resolution_clock::time_point start_time;

    public:
        btm_timer() : start_time(std::chrono::high_resolution_clock::now()) {}

        void reset() {
            start_time = std::chrono::high_resolution_clock::now();
        }
        void start() {
            reset();
        }
        void stop() {
            // No action needed for stop in this implementation
        }
        void advance() {
            // No action needed for advance in this implementation
        }

        double elapsed() {
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end_time - start_time;
            start_time = end_time; // Reset the start time for the next measurement
            return diff.count();
        }
    };

    static btm_timer timer;

    void start_timer() {timer.start();};
    double get_elapsed_time() {
        return timer.elapsed();
    }
    void stop_timer() {timer.stop();};
}
