#pragma once

#include <string>

namespace Beer::Rendering
{
    constexpr std::string_view MAIN_COLOR = "MainColor";
    constexpr std::string_view MAIN_DEPTH = "MainDepth";

    constexpr std::string_view OPAQUE_PASS = "Opaque";
    constexpr std::string_view SKYBOX_PASS = "Skybox";
} // namespace Beer::Rendering
