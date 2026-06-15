#include "System/Base/Clock/Clock.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Base/Clock/ClockManager.hpp"
#include <print>

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

    std::shared_ptr<Timer> Clock::Timer(float duration)
    {
        return clockManager->CreateTimer(duration);
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

    void Clock::BindToShaders()
    {
        Rendering::Shader::Globals()->SetTime(time, deltaTime);
    }
} // namespace Beer::System
