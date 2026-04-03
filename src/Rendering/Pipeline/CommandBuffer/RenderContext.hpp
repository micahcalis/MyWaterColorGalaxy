#pragma once

#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Pipeline/Frame/FrameBlackbox.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "System/Camera/Camera.hpp"
#include "System/Drawing/RenderRegister.hpp"
#include "System/Light/ILight.hpp"

namespace Beer::Rendering
{
    struct RenderContext
    {
    public:
        System::Camera* Camera = nullptr;
        System::ILight* MainLight = nullptr;
        FrameBlackbox* BlackBox = nullptr;
        RenderTexture* MainColorTarget = nullptr;
        RenderTexture* MainDepthTarget = nullptr;
        PhaseBuffer* TransformBuffer = nullptr;
        System::RenderRegister* Register = nullptr;
    };
} // namespace Beer::Rendering
