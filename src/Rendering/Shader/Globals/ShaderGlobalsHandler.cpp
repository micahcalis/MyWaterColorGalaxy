#include "Rendering/Shader/Globals/ShaderGlobalsHandler.hpp"
#include "EngineGlobalBuffer.hpp"
#include "glm/matrix.hpp"

namespace Beer::Rendering
{
    ShaderGlobalsHandler::ShaderGlobalsHandler()
    {
        engineGlobals = std::make_unique<EngineGlobalBuffer>();
    }

    void ShaderGlobalsHandler::Update()
    {
        engineGlobals->Update(engineGlobalsData);
    }

    void ShaderGlobalsHandler::SetTime(float time, float deltaTime)
    {
        engineGlobalsData.Time = time;
        engineGlobalsData.DeltaTime = deltaTime;
    }

    void ShaderGlobalsHandler::SetCamera(const glm::mat4 viewMat, const glm::mat4 projMat, glm::vec3 cameraPos)
    {
        glm::mat4 viewProjMat = projMat * viewMat;
        engineGlobalsData.ViewProjMat = viewProjMat;
        engineGlobalsData.InvViewProjMat = glm::inverse(viewProjMat);

        engineGlobalsData.CameraPos = cameraPos;
    }

    void ShaderGlobalsHandler::SetScreen(float width, float height)
    {
        engineGlobalsData.ScreenParams = glm::vec4(width, height, 1.0f / width, 1.0f / height);
    }
} // namespace Beer::Rendering

