#pragma once

#include "System/Base/Clock/ClockManager.hpp"
#include <memory>

namespace Beer::System
{
    class GameManager
    {
    private:
        std::unique_ptr<ClockManager> clockManager = nullptr;

    public:
        void InitializeGame();
        void Update();

    private:
        void InitializeBase();
        void UpdateBase();
    };
} // namespace Beer::System
