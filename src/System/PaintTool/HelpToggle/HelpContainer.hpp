#pragma once

#include "HelpButtonSubEntity.hpp"
#include "System/PaintTool/HelpToggle/HelpToggle.hpp"
namespace Beer::System
{
    class HelpContainer
    {
    private:
        std::unique_ptr<HelpToggle> helpToggle = nullptr;

    public:
        void SetHelpToggle(HelpButtonSubEntity* helpEntity,
            Function<void> markDirty)
        {
            helpToggle = std::make_unique<HelpToggle>(helpEntity, markDirty);
        }
    };
} // namespace Beer::System
