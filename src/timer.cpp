#define NOMINMAX
#include <windows.h>
#include "glew.h"
#include "timer.h"

namespace btm_framework {
    /**
     * @class gl_timer
     * @brief High-resolution timer for measuring elapsed time and system time.
     *
     * This class provides functionality to start, stop, reset, and advance a timer,
     * as well as retrieve absolute and elapsed times with high precision.
     * Platform-specific implementations are provided for Windows and POSIX systems.
     */
    class gl_timer
    {
    public:
        /**
         * @brief Constructs a new gl_timer object and initializes the timer.
         */
        gl_timer();

        /**
         * @brief Resets the timer to its initial state.
         */
        void reset();

        /**
         * @brief Starts or resumes the timer.
         */
        void start();

        /**
         * @brief Stops or pauses the timer.
         */
        void stop();

        /**
         * @brief Advances the timer by 0.1 seconds.
         */
        void advance();

        /**
         * @brief Gets the absolute system time in seconds.
         * @return The absolute system time as a double.
         */
        double get_absolute_time();

        /**
         * @brief Gets the current timer value in seconds.
         * @return The current timer value as a double.
         */
        double get_time();

        /**
         * @brief Gets the time elapsed since the last call to get_elapsed_time().
         * @return The elapsed time in seconds as a double.
         */
        double get_elapsed_time();

        /**
         * @brief Checks if the timer is currently stopped.
         * @return True if the timer is stopped, false otherwise.
         */
        bool is_stopped();

    protected:
        /// Indicates whether the timer is currently stopped.
        bool m_bTimerStopped;

        /// Indicates if QueryPerformanceFrequency is used.
        bool m_bUsingQPF;
        /// Ticks per second for QueryPerformanceFrequency.
        LONGLONG m_llQPFTicksPerSec;

        /// Time when the timer was stopped.
        LONGLONG m_llStopTime;
        /// Last elapsed time value.
        LONGLONG m_llLastElapsedTime;
        /// Base time for the timer.
        LONGLONG m_llBaseTime;
    };




    /**
     * @brief Constructs a gl_timer object and initializes timer state.
     * 
     * Initializes internal variables and attempts to use the high-resolution
     * performance counter if available.
     */
    gl_timer::gl_timer()
    {
        m_bTimerStopped = true;
        m_bUsingQPF = false;
        m_llQPFTicksPerSec = 0;
        m_llStopTime = 0;
        m_llLastElapsedTime = 0;
        m_llBaseTime = 0;

        LARGE_INTEGER qwTicksPerSec;
        m_bUsingQPF = (bool)(QueryPerformanceFrequency(&qwTicksPerSec) != 0);
        m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
    }

    /**
     * @brief Resets the timer to the current time.
     * 
     * Sets the base time and last elapsed time to the current value of the
     * performance counter. If the timer is stopped, uses the stop time.
     */
    void gl_timer::reset()
    {
        if (!m_bUsingQPF)
            return;

        LARGE_INTEGER qwTime;
        if (m_llStopTime != 0)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter(&qwTime);

        m_llBaseTime = qwTime.QuadPart;
        m_llLastElapsedTime = qwTime.QuadPart;
        m_llStopTime = 0;
        m_bTimerStopped = FALSE;
    }

    /**
     * @brief Starts or resumes the timer.
     * 
     * If the timer was previously stopped, adjusts the base time to account for
     * the paused duration and resumes timing.
     */
    void gl_timer::start()
    {
        if (!m_bUsingQPF)
            return;

        LARGE_INTEGER qwTime;
        QueryPerformanceCounter(&qwTime);

        if (m_bTimerStopped)
            m_llBaseTime += qwTime.QuadPart - m_llStopTime;
        m_llStopTime = 0;
        m_llLastElapsedTime = qwTime.QuadPart;
        m_bTimerStopped = FALSE;
    }

    /**
     * @brief Stops or pauses the timer.
     * 
     * Records the current time as the stop time and marks the timer as stopped.
     */
    void gl_timer::stop()
    {
        if (!m_bUsingQPF)
            return;

        if (!m_bTimerStopped)
        {
            LARGE_INTEGER qwTime;
            if (m_llStopTime != 0)
                qwTime.QuadPart = m_llStopTime;
            else
                QueryPerformanceCounter(&qwTime);

            m_llStopTime = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            m_bTimerStopped = TRUE;
        }
    }

    /**
     * @brief Advances the timer by a fixed interval (0.1 seconds).
     * 
     * Useful for simulating time progression when the timer is stopped.
     */
    void gl_timer::advance()
    {
        if (!m_bUsingQPF)
            return;
        m_llStopTime += m_llQPFTicksPerSec / 10;
    }

    /**
     * @brief Gets the absolute system time in seconds.
     * 
     * Returns the current value of the performance counter in seconds.
     * 
     * @return double Absolute time in seconds, or -1.0 if unavailable.
     */
    double gl_timer::get_absolute_time()
    {
        if (!m_bUsingQPF)
            return -1.0;

        LARGE_INTEGER qwTime;
        if (m_llStopTime != 0)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter(&qwTime);

        double fTime = qwTime.QuadPart / (double)m_llQPFTicksPerSec;

        return fTime;
    }

    /**
     * @brief Gets the application time in seconds since the last reset.
     * 
     * Returns the elapsed time since the timer was last reset or started.
     * 
     * @return double Application time in seconds, or -1.0 if unavailable.
     */
    double gl_timer::get_time()
    {
        if (!m_bUsingQPF)
            return -1.0;

        LARGE_INTEGER qwTime;
        if (m_llStopTime != 0)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter(&qwTime);

        double fAppTime = (double)(qwTime.QuadPart - m_llBaseTime) / (double)m_llQPFTicksPerSec;

        return fAppTime;
    }

    /**
     * @brief Gets the time elapsed since the last call to get_elapsed_time().
     * 
     * Updates the last elapsed time to the current time.
     * 
     * @return double Elapsed time in seconds, or -1.0 if unavailable.
     */
    double gl_timer::get_elapsed_time()
    {
        if (!m_bUsingQPF)
            return -1.0;

        LARGE_INTEGER qwTime;
        if (m_llStopTime != 0)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter(&qwTime);

        double fElapsedTime = (double)(qwTime.QuadPart - m_llLastElapsedTime) / (double)m_llQPFTicksPerSec;
        m_llLastElapsedTime = qwTime.QuadPart;

        return fElapsedTime;
    }

    /**
     * @brief Checks if the timer is currently stopped.
     * 
     * @return true if the timer is stopped, false otherwise.
     */
    bool gl_timer::is_stopped()
    {
        return m_bTimerStopped;
    }

    static gl_timer timer;
    void reset_timer(){
        timer.reset();
    }
    void start_timer(){
        timer.start();
    }
    void stop_timer(){
        timer.stop();
    }
    void advance_timer(){
        timer.advance();
    }
    double get_absolute_time(){
        return timer.get_absolute_time();
    }
    double get_time(){
        return timer.get_time();
    }
    double get_elapsed_time(){
        return timer.get_elapsed_time();
    }
    bool is_timer_stopped(){
        return timer.is_stopped();
    }

}