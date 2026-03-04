#pragma once

namespace Beer::System
{
    class ClockManager;
}

namespace Beer::System
{
    class Clock
    {
    private:
        inline static ClockManager* clockManager = nullptr;

    private:
        const double startTime;
        double time = 0;
        float deltaTime = 0;

    public:
        static void SetClockManager(ClockManager* clockManager);
        static double Time();
        static double DeltaTime();

        Clock(double startTime);
        double GetTime() const { return time; }
        double GetDeltaTime() const { return deltaTime; }
        void SetTime(double currentTime);
    };
} // namespace Beer::System
