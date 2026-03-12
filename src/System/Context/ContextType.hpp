#pragma once

#include "glm/fwd.hpp"
#include <array>

namespace Beer::System
{
    enum class ContextType : uint32_t
    {
        None = 0,
        LoadingScreen = 1 << 0,
        MainMenu = 1 << 1,
        SettingsMenu = 1 << 2,
        PaintTool = 1 << 3,
        Galaxy = 1 << 4,
        All = ~0u
    };

    inline constexpr uint32_t operator|(ContextType a, ContextType b)
    {
        return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
    }

    inline constexpr uint32_t operator|(uint32_t mask, ContextType context)
    {
        return mask | static_cast<uint32_t>(context);
    }

    constexpr uint32_t CTXT_NONE_BITS = static_cast<uint32_t>(ContextType::None);
    constexpr uint32_t CTXT_LOADING_SCREEN_BITS = static_cast<uint32_t>(ContextType::LoadingScreen);
    constexpr uint32_t CTXT_MAIN_MENU_BITS = static_cast<uint32_t>(ContextType::MainMenu);
    constexpr uint32_t CTXT_SETTINGS_MENU_BITS = static_cast<uint32_t>(ContextType::SettingsMenu);
    constexpr uint32_t CTXT_PAINT_TOOL_BITS = static_cast<uint32_t>(ContextType::PaintTool);
    constexpr uint32_t CTXT_GALAXY_BITS = static_cast<uint32_t>(ContextType::Galaxy);
    constexpr uint32_t CTXT_ALL_BITS = static_cast<uint32_t>(ContextType::All);

    constexpr std::array<ContextType, 5> AVAILABLE_CONTEXTS = {
        ContextType::LoadingScreen,
        ContextType::MainMenu,
        ContextType::SettingsMenu,
        ContextType::PaintTool,
        ContextType::Galaxy};
} // namespace Beer::System
