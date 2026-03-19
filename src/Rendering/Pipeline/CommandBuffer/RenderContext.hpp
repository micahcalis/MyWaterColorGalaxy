#pragma once

#include "Rendering/Pipeline/Frame/FrameBlackbox.hpp"
#include "System/Camera/Camera.hpp"
#include "System/Light/ILight.hpp"

namespace Beer::Rendering
{
    struct RenderContext
    {
    public:
        System::Camera* Camera = nullptr;
        System::ILight* MainLight = nullptr;
        FrameBlackbox* BlackBox = nullptr;
    };
} // namespace Beer::Rendering
