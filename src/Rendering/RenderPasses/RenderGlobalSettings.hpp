#pragma once

#include <string>

namespace Beer::Rendering
{
    constexpr std::string_view MAIN_COLOR = "MainColor";
    constexpr std::string_view MAIN_DEPTH = "MainDepth";

    constexpr std::string_view OPAQUE_PASS = "Opaque";
    constexpr std::string_view SKYBOX_PASS = "Skybox";

    constexpr static uint32_t TRANSFORM_BUFFER_COUNT = 100'000;
    constexpr std::string_view TRANSFORM_BUFFER_NAME = "TransformInstancingBuffer";
} // namespace Beer::Rendering
