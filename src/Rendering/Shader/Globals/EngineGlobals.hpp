#pragma once

#include <glm/glm.hpp>
#include "Rendering/Shader/Globals/PaddingMacros.hpp"
#include "glm/matrix.hpp"

namespace Beer::Rendering
{
    struct ALIGN_16 EngineGlobals
    {
    public:
        const static int BINDING = 0;

    private:
        float time;
        float deltaTime;
        PAD_8(0);

        ALIGN_16 glm::mat4 viewProjMat;
        ALIGN_16 glm::mat4 invViewProjMat;

        ALIGN_16 glm::vec3 cameraPos;
        PAD_4(1);

        ALIGN_16 glm::vec4 screenParams;

    public:
        void SetTime(float time, float deltaTime)
        {
            this->time = time;
            this->deltaTime = time;
        }

        void SetCamera(const glm::mat4 viewMat, const glm::mat4 projMat, glm::vec3 cameraPos)
        {
            viewProjMat = projMat * viewMat;
            invViewProjMat = glm::inverse(viewProjMat);
            this->cameraPos = cameraPos;
        }

        void SetScreen(float width, float height)
        {
            screenParams = glm::vec4(width, height, 1.0f / width, 1.0f / height);
        }
    };
} // namespace Beer::Rendering
