//
// StepTimer.h - A simple timer that provides elapsed time information
//

#pragma once

#include <cmath>
#include <exception>
#include <stdint.h>
#include <profileapi.h>

namespace buma
{

// Helper class for animation and simulation timing.
class StepTimer
{
public:
    StepTimer() noexcept(false)
    #pragma region initialize member variables
        : elapsed_ticks             (0)
        , total_ticks               (0)
        , left_over_ticks           (0)
        , frame_count               (0)
        , frames_per_second         (0)
        , frames_this_second        (0)
        , qpc_second_counter        (0)
        , is_fixed_time_step        (false)
        , target_elapsed_ticks      (ticks_per_second / 60)
        , is_one_sec_elapsed        (false)
#pragma endregion
    {
        if (!QueryPerformanceFrequency(&qpc_frequency))
        {
            throw std::exception("QueryPerformanceFrequency");
        }

        if (!QueryPerformanceCounter(&qpc_last_time))
        {
            throw std::exception("QueryPerformanceCounter");
        }

        // Initialize max delta to 1/10 of a second.
        qpc_max_delta = static_cast<uint64_t>(qpc_frequency.QuadPart / 10);
    }

    // Get elapsed time since the previous Update call.
    const uint64_t& GetElapsedTicks()               const                { return elapsed_ticks; }
    double          GetElapsedSeconds()             const                { return TicksToSeconds(elapsed_ticks); }
    double          GetElapsedMilliseconds()        const                { return TicksToSeconds(elapsed_ticks)  * 1000.0; }
    float           GetElapsedSecondsF()            const                { return TicksToSecondsF(elapsed_ticks); }
    float           GetElapsedMillisecondsF()       const                { return TicksToSecondsF(elapsed_ticks) * 1000.f; }

    // Get total time since the start of the program.
    const uint64_t& GetTotalTicks()                 const                { return total_ticks; }
    double          GetTotalSeconds()               const                { return TicksToSeconds(total_ticks); }

    // Get total number of updates since start of the program.
    const uint32_t& GetFrameCount()                 const                { return frame_count; }

    // Get the current framerate.
    const uint32_t& GetFramesPerSecond()            const                { return frames_per_second; }

    const bool      IsOneSecElapsed()               const                { return is_one_sec_elapsed; }

    // Set whether to use fixed or variable timestep mode.
    void SetIsFixedTimeStep(const bool _is_fixed_time_step)            { is_fixed_time_step = _is_fixed_time_step; }

    // Set how often to call Update when in fixed timestep mode.
    void SetTargetElapsedTicks(const uint64_t& _target_elapsed)        { target_elapsed_ticks = _target_elapsed; }
    void SetTargetElapsedSeconds(const double& _target_elapsed)        { target_elapsed_ticks = SecondsToTicks(_target_elapsed); }

    // Integer format represents time using 10,000,000 ticks per second.
    static const uint64_t ticks_per_second = 10000000;

    static double    TicksToSeconds     (const uint64_t& _ticks) { return static_cast<double>  (_ticks) / ticks_per_second; }
    static float     TicksToSecondsF    (const uint64_t& _ticks) { return static_cast<float>   (_ticks) / static_cast<float>(ticks_per_second); }
    static uint64_t  SecondsToTicks     (const double& _seconds) { return static_cast<uint64_t>(_seconds * ticks_per_second); }

    // After an intentional timing discontinuity (for instance a blocking IO operation)
    // call this to avoid having the fixed timestep logic attempt a set of catch-up
    // Update calls.

    void ResetElapsedTime()
    {
        if (!QueryPerformanceCounter(&qpc_last_time))
        {
            throw std::exception("QueryPerformanceCounter");
        }
        
        left_over_ticks        = 0;
        frames_per_second      = 0;
        frames_this_second     = 0;
        qpc_second_counter     = 0;
    }
    
    // Update timer state, calling the specified Update function the appropriate number of times.
    void Tick()
    {
        // Query the current time.
        LARGE_INTEGER current_time{};
        
        if (!QueryPerformanceCounter(&current_time))
        {
            throw std::exception("QueryPerformanceCounter");
        }
        
        uint64_t time_delta = static_cast<uint64_t>(current_time.QuadPart - qpc_last_time.QuadPart);
        
        qpc_last_time        = current_time;
        qpc_second_counter += time_delta;
        
        // Clamp excessively large time deltas (e.g. after paused in the debugger).
        if (time_delta > qpc_max_delta)
        {
            time_delta = qpc_max_delta;
        }
        
        // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
        time_delta *= ticks_per_second;
        time_delta /= static_cast<uint64_t>(qpc_frequency.QuadPart);
        
        uint32_t last_frame_count = frame_count;
        
        if (is_fixed_time_step)
        {
            // Fixed timestep update logic
            
            /*
                If the app is running very close to the target elapsed time (within 1/4 of a millisecond) just clamp the clock to
                exactly match the target value.
                This prevents tiny and irrelevant errors from accumulating over time.
                Without this clamping, a game that requested a 60 fps fixed update,
                running with vsync enabled on a 59.94 NTSC display, would eventually
                accumulate enough tiny errors that it would drop a frame.
                It is better to just round small deviations down to zero to leave things running smoothly.
            */
            if (static_cast<uint64_t>(std::llabs(static_cast<int64_t>(time_delta - target_elapsed_ticks))) < ticks_per_second / 4000)
            {
                time_delta = target_elapsed_ticks;
            }
            
            left_over_ticks += time_delta;
            
            while (left_over_ticks >= target_elapsed_ticks)
            {
                elapsed_ticks     = target_elapsed_ticks;
                total_ticks        += target_elapsed_ticks;
                left_over_ticks -= target_elapsed_ticks;
                frame_count++;
            }
        }
        else
        {
            // Variable timestep update logic.
            elapsed_ticks     = time_delta;
            total_ticks        += time_delta;
            left_over_ticks  = 0;
            frame_count++;
        }
        
        // Track the current framerate.
        if (frame_count != last_frame_count)
        {
            frames_this_second++;
        }
        
        if (qpc_second_counter >= static_cast<uint64_t>(qpc_frequency.QuadPart))
        {
            frames_per_second    = frames_this_second;
            frames_this_second    = 0;
            qpc_second_counter %= static_cast<uint64_t>(qpc_frequency.QuadPart);
            is_one_sec_elapsed    = true;
        }
        else
            is_one_sec_elapsed = false;
    }

private:
    // Source timing data uses QPC units.
    // https://ja.wikipedia.org/wiki/%E9%87%8F%E5%AD%90%E3%83%9D%E3%82%A4%E3%83%B3%E3%83%88%E3%82%B3%E3%83%B3%E3%82%BF%E3%82%AF%E3%83%88#%E5%BF%9C%E7%94%A8
    LARGE_INTEGER    qpc_frequency;
    LARGE_INTEGER    qpc_last_time;
    uint64_t        qpc_max_delta;

    // Derived timing data uses a canonical tick format.
    uint64_t        elapsed_ticks;
    uint64_t        total_ticks;
    uint64_t        left_over_ticks;

    // Members for tracking the framerate.
    uint32_t        frame_count;
    uint32_t        frames_per_second;
    uint32_t        frames_this_second;
    uint64_t        qpc_second_counter;

    // Members for configuring fixed timestep mode.
    bool            is_fixed_time_step;
    uint64_t        target_elapsed_ticks;

    // for one sec calculation
    bool            is_one_sec_elapsed;
};


}// namespace buma
