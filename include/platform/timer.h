/* high resolution timer */
#ifndef __timer_h__
#define __timer_h__

namespace btm {

    void start_timer();
    double get_elapsed_time();
    void stop_timer();
}

#endif // __timer_h__
