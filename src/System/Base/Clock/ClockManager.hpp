#pragma once

#include "System/Base/Clock/Clock.hpp"
#include <chrono>
#include <vector>

namespace Beer::System
{
    class ClockManager
    {
    private:
        Clock clock;
        std::vector<std::shared_ptr<Timer>> activeTimers;

    public:
        ClockManager();
        void Update();
        double GetTime() const { return clock.GetTime(); }
        float GetDeltaTime() const { return clock.GetDeltaTime(); }
        std::shared_ptr<Timer> CreateTimer(float duration);

    private:
        double TimeToSeconds(std::chrono::time_point<std::chrono::high_resolution_clock> timePoint);
    };
} // namespace Beer::System
