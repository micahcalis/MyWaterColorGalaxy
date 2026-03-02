#pragma once

#include "Core/Application/Renderer/Device.hpp"
#include "EngineGlobalBuffer.hpp"
#include <memory>

namespace Beer::Rendering
{
    class ShaderGlobalsHandler
    {
    private:
        std::unique_ptr<EngineGlobalBuffer> engineGlobals = nullptr;
        EngineGlobals engineGlobalsData;

    public:
        ShaderGlobalsHandler();
        void Update();
        void SetTime(float time, float deltaTime);
        void SetCamera(const glm::mat4 viewMat, const glm::mat4 projMat, glm::vec3 cameraPos);
        void SetScreen(float width, float height);
    };
} // namespace Beer::Rendering
