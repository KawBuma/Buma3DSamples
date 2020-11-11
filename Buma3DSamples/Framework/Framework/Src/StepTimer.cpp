#include "pch.h"
#include "StepTimer.h"
#include <profileapi.h>

namespace buma
{

StepTimer::StepTimer() noexcept(false)
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
{
    if (!QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&qpc_frequency_quad_part)))
        throw std::exception("QueryPerformanceFrequency");

    if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&qpc_last_time_quad_part)))
        throw std::exception("QueryPerformanceCounter");

    // Initialize max delta to 1/10 of a second.
    qpc_max_delta = static_cast<uint64_t>(qpc_frequency_quad_part / 10);
}

void StepTimer::ResetElapsedTime()
{
    if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&qpc_frequency_quad_part)))
    {
        throw std::exception("QueryPerformanceCounter");
    }

    left_over_ticks = 0;
    frames_per_second = 0;
    frames_this_second = 0;
    qpc_second_counter = 0;
}

// Update timer state, calling the specified Update function the appropriate number of times.

void StepTimer::Tick()
{
    // Query the current time.
    LARGE_INTEGER current_time{};

    if (!QueryPerformanceCounter(&current_time))
    {
        throw std::exception("QueryPerformanceCounter");
    }

    uint64_t time_delta = static_cast<uint64_t>(current_time.QuadPart - qpc_last_time_quad_part);

    qpc_last_time_quad_part = current_time.QuadPart;
    qpc_second_counter += time_delta;

    // Clamp excessively large time deltas (e.g. after paused in the debugger).
    if (time_delta > qpc_max_delta)
    {
        time_delta = qpc_max_delta;
    }

    // Convert QPC units into a canonical tick format. This cannot overflow due to the previous clamp.
    time_delta *= ticks_per_second;
    time_delta /= static_cast<uint64_t>(qpc_frequency_quad_part);

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
            elapsed_ticks = target_elapsed_ticks;
            total_ticks += target_elapsed_ticks;
            left_over_ticks -= target_elapsed_ticks;
            frame_count++;
        }
    }
    else
    {
        // Variable timestep update logic.
        elapsed_ticks = time_delta;
        total_ticks += time_delta;
        left_over_ticks = 0;
        frame_count++;
    }

    // Track the current framerate.
    if (frame_count != last_frame_count)
    {
        frames_this_second++;
    }

    if (qpc_second_counter >= static_cast<uint64_t>(qpc_frequency_quad_part))
    {
        frames_per_second = frames_this_second;
        frames_this_second = 0;
        qpc_second_counter %= static_cast<uint64_t>(qpc_frequency_quad_part);
        is_one_sec_elapsed = true;
    }
    else
        is_one_sec_elapsed = false;
}




}// namespace buma
