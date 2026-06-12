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
        clock.BindToShaders();

        std::erase_if(activeTimers, [this](const auto& timer) -> bool {
            return timer->completed;
        });

        for (auto& timer : activeTimers)
        {
            timer->Update();
        }
    }

    std::shared_ptr<Timer> ClockManager::CreateTimer(float duration)
    {
        std::shared_ptr<Timer> timer(new Timer(duration));
        activeTimers.push_back(timer);
        return timer;
    }

    double ClockManager::TimeToSeconds(std::chrono::time_point<std::chrono::high_resolution_clock> timePoint)
    {
        return std::chrono::duration<double>(timePoint.time_since_epoch()).count();
    }
} // namespace Beer::System
