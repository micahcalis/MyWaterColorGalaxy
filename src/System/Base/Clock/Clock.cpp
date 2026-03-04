#include "System/Base/Clock/Clock.hpp"
#include "System/Base/Clock/ClockManager.hpp"

namespace Beer::System
{
    void Clock::SetClockManager(ClockManager* clockManager)
    {
        Clock::clockManager = clockManager;
    }

    double Clock::Time()
    {
        if (!clockManager)
            return 0.0;

        return clockManager->GetTime();
    }

    double Clock::DeltaTime()
    {
        if (!clockManager)
            return 0.0;

        return clockManager->GetDeltaTime();
    }

    Clock::Clock(double startTime)
        : startTime(startTime)
    {
        time = startTime;
    }

    void Clock::SetTime(double currentTime)
    {
        double newTime = currentTime - startTime;
        deltaTime = newTime - time;
        time = newTime;
    }
} // namespace Beer::System
