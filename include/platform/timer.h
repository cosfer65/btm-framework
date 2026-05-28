/* high resolution timer */
#ifndef __timer_h__
#define __timer_h__

namespace btm {
    void reset_timer();
    void start_timer();
    void stop_timer();
    void advance_timer();
    double get_absolute_time();
    double get_time();
    double get_elapsed_time();
    bool is_timer_stopped();
}

#endif // __timer_h__
