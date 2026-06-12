#pragma once

#include "System/Delegates/BeerEvent.hpp"

namespace Beer::System
{
    class Timer
    {
        friend class ClockManager;

    public:
        BeerEvent<void()> OnTimerComplete;

    private:
        float duration;
        float currentTime;
        bool started = false;
        float startTime = 0;
        bool completed = false;

    public:
        void Start();

        void ForceEnd()
        {
            if (completed)
                return;

            currentTime = duration;
            completed = true;
            OnTimerComplete.Invoke();
        }

        float Duration() const { return duration; }
        float CurrentTime() const { return currentTime; }

    private:
        Timer(float duration);

        void Update();
    };

} // namespace Beer::System
