#pragma once

#include "Rendering/Material/Material.hpp"
#include "glm/fwd.hpp"

namespace Beer::Rendering
{
    class DynamicCanvasUpdater
    {
    private:
        glm::vec2 accumulatedCanvasOffset = glm::vec2(0);
        float panFraction;
        glm::vec3 previousForward;
        Material* material = nullptr;

    public:
        DynamicCanvasUpdater(Material* material,
            const glm::vec3 forward,
            float panPercentage)
            : panFraction(panPercentage * 0.01f)
            , previousForward(forward)
            , material(material)
        {
        }

        void Update(const glm::vec3 forward, const glm::vec3 up, const glm::vec3 right, const float fov)
        {
            UpdateOffset(forward, up, right, fov);
            UpdateMaterial();
        }

    private:
        void UpdateOffset(const glm::vec3& forward, const glm::vec3& up, const glm::vec3& right, const float fov)
        {
            float x = glm::dot(previousForward, right);
            float y = glm::dot(previousForward, up);
            float z = glm::dot(previousForward, forward);

            float deltaYaw = glm::degrees(std::atan2(-x, z));
            float deltaPitch = glm::degrees(std::atan2(-y, z));

            float uvDeltaX = (deltaYaw / fov) * panFraction;
            float uvDeltaY = (deltaPitch / fov) * panFraction;

            accumulatedCanvasOffset.x += uvDeltaX;
            accumulatedCanvasOffset.y += uvDeltaY;

            accumulatedCanvasOffset.x = std::fmod(accumulatedCanvasOffset.x, 1.0f);
            accumulatedCanvasOffset.y = std::fmod(accumulatedCanvasOffset.y, 1.0f);

            if (accumulatedCanvasOffset.x < 0.0f)
                accumulatedCanvasOffset.x += 1.0f;
            if (accumulatedCanvasOffset.y < 0.0f)
                accumulatedCanvasOffset.y += 1.0f;

            previousForward = forward;
        }

        void UpdateMaterial()
        {
            material->SetVector("_DynamicOffset", glm::vec4(accumulatedCanvasOffset, 0, 0));
        }
    };
} // namespace Beer::Rendering
