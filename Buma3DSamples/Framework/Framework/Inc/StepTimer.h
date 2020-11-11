//
// StepTimer.h - A simple timer that provides elapsed time information
//

#pragma once

#include <cmath>
#include <exception>
#include <cstdint>

namespace buma
{

// Helper class for animation and simulation timing.
class StepTimer
{
public:
    StepTimer() noexcept(false);

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

    void ResetElapsedTime();
    
    // Update timer state, calling the specified Update function the appropriate number of times.
    void Tick();

private:
    // Source timing data uses QPC units.
    // https://ja.wikipedia.org/wiki/%E9%87%8F%E5%AD%90%E3%83%9D%E3%82%A4%E3%83%B3%E3%83%88%E3%82%B3%E3%83%B3%E3%82%BF%E3%82%AF%E3%83%88#%E5%BF%9C%E7%94%A8
    int64_t         qpc_frequency_quad_part;
    int64_t         qpc_last_time_quad_part;
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
