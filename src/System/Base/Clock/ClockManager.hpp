#pragma once

#include "System/Base/Clock/Clock.hpp"
#include <chrono>

namespace Beer::System
{
    class ClockManager
    {
    private:
        Clock clock;

    public:
        ClockManager();
        void Update();
        double GetTime() const { return clock.GetTime(); }
        float GetDeltaTime() const { return clock.GetDeltaTime(); }

    private:
        double TimeToSeconds(std::chrono::time_point<std::chrono::high_resolution_clock> timePoint);
    };
} // namespace Beer::System
