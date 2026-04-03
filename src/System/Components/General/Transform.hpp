#pragma once

#include "Rendering/Shader/Globals/ModelTransformData.hpp"
#include "Rendering/Shader/ModelPush.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Beer::System
{
    struct Transform
    {
    public:
        glm::vec3 Position = glm::vec3(0);
        glm::quat Rotation = glm::identity<glm::quat>();
        glm::vec3 Scale = glm::vec3(1);

    public:
        glm::vec3 GetForward() const
        {
            return Rotation * glm::vec3(0, 0, 1);
        }

        glm::vec3 GetRight() const
        {
            return Rotation * glm::vec3(1, 0, 0);
        }

        glm::vec3 GetUp() const
        {
            return Rotation * glm::vec3(0, 1, 0);
        }

        [[nodiscard]] Rendering::ModelPush GetShaderTransform() const
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), Position);
            model *= glm::toMat4(Rotation);
            model = glm::scale(model, Scale);
            return Rendering::ModelPush(model, glm::transpose(glm::inverse(model)), false);
        }

        [[nodiscard]] Rendering::ModelTransformData GetModelTransformData() const
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), Position);
            model *= glm::toMat4(Rotation);
            model = glm::scale(model, Scale);

            return {model, glm::transpose(glm::inverse(model))};
        }

    public:
        static std::vector<Rendering::ModelTransformData> ToModelData(const std::vector<Transform>& transforms)
        {
            std::vector<Rendering::ModelTransformData> data;
            data.reserve(transforms.size());

            for (const auto& transform : transforms)
            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.Position);
                model *= glm::toMat4(transform.Rotation);
                model = glm::scale(model, transform.Scale);

                data.emplace_back(model, glm::transpose(glm::inverse(model)));
            }

            return data;
        }
    };
} // namespace Beer::System
