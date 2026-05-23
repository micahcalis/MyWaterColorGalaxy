#pragma once

#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Pipeline/Frame/FrameBlackbox.hpp"
#include "Rendering/RenderPasses/RenderGlobalSettings.hpp"
#include "Rendering/Shader/FragmentOutput.hpp"
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
        RenderTexture* MainDepthTarget = nullptr;
        PhaseBuffer* TransformBuffer = nullptr;
        System::RenderRegister* Register = nullptr;
        FragmentOutput Output{};

    private:
        RenderTexture* mainColorTargetA = nullptr;
        RenderTexture* mainColorTargetB = nullptr;

        mutable bool pong = false;

    public:
        void SetMainColorTargets(RenderTexture* a, RenderTexture* b)
        {
            mainColorTargetA = a;
            mainColorTargetB = b;
        }

        void RegisterMainColorPongPass() const
        {
            pong = !pong;
        }

        [[nodiscard]] RenderTexture* GetMainColorSource() const
        {
            return pong ? mainColorTargetA : mainColorTargetB;
        }

        [[nodiscard]] RenderTexture* GetMainColorDestination() const
        {
            return pong ? mainColorTargetB : mainColorTargetA;
        }

        [[nodiscard]] bool GetPongState() const { return pong; }

        const std::string& GetMainColorSourceName() const
        {
            return pong ? MAIN_COLOR_A : MAIN_COLOR_B;
        }

        const std::string& GetMainColorDestinationName() const
        {
            return pong ? MAIN_COLOR_B : MAIN_COLOR_A;
        }

        void ResetMainColorPong() const
        {
            pong = false;
        }
    };
} // namespace Beer::Rendering
