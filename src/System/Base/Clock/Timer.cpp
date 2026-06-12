#include "System/Base/Clock/Timer.hpp"
#include "System/Base/Clock/Clock.hpp"

namespace Beer::System
{
    Timer::Timer(float duration)
        : duration(duration)
    {
    }

    void Timer::Start()
    {
        started = true;
        startTime = Clock::Time();
    }

    void Timer::Update()
    {
        if (!started || completed)
            return;

        currentTime = (float)Clock::Time() - startTime;

        if (currentTime >= duration)
        {
            currentTime = duration;
            completed = true;
            OnTimerComplete.Invoke();
        }
    }
} // namespace Beer::System
