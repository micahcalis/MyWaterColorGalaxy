#include "System/Base/Clock/ClockManager.hpp"
#include <chrono>

namespace Beer::System
{
    ClockManager::ClockManager()
        : clock(Clock(TimeToSeconds(std::chrono::high_resolution_clock::now())))
    {
        Clock::SetClockManager(this);
    }

    void ClockManager::Update()
    {
        clock.SetTime(TimeToSeconds(std::chrono::high_resolution_clock::now()));
    }

    double ClockManager::TimeToSeconds(std::chrono::time_point<std::chrono::high_resolution_clock> timePoint)
    {
        return std::chrono::duration<double>(timePoint.time_since_epoch()).count();
    }
} // namespace Beer::System
