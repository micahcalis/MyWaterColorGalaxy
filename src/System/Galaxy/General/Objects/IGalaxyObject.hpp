#pragma once

#include "Rendering/Mesh/MeshDrawInfo.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Galaxy/General/GalaxyTransformSettings.hpp"
#include "System/Galaxy/General/Objects/OrbitComponent.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Shader/ShaderPass.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <memory>

namespace Beer::System
{
    class IGalaxyObject
    {
    protected:
        std::unique_ptr<GameSubEntity> subEntity = nullptr;
        std::unique_ptr<Rendering::Material> material = nullptr;
        std::unique_ptr<OrbitComponent> orbitComponent = nullptr;

    public:
        virtual ~IGalaxyObject() = default;

        IGalaxyObject(const SerializableGalaxyComponent& serializedData,
            const GalaxyTransformSettings& settings,
            const char* shaderPath)
        {
            Transform transform{};
            transform.Scale = glm::vec3(serializedData.Scale * settings.SizeScale);
            glm::vec2 posXZ = serializedData.Position * settings.PositionScale;
            transform.Position = glm::vec3(posXZ.x, 0, posXZ.y);
            subEntity = std::make_unique<GameSubEntity>(transform);

            material = std::make_unique<Rendering::Material>(shaderPath);

            orbitComponent = std::make_unique<OrbitComponent>(subEntity->GetTransform(),
                0.1f,                      // TEMPORARILY HARDCODED, NEED FIX
                OrbitDirection::ClockWise, // TEMPORARILY HARDCODED, NEED FIX
                settings.SunCenter,
                glm::vec2(1.0f),       // TEMPORARILY HARDCODED, NEED FIX
                glm::vec2(0.2f, 0.1f), // TEMPORARILY HARDCODED, NEED FIX
                glm::vec3(serializedData.Position.x, 0, serializedData.Position.y));
        }

        virtual void Update()
        {
            orbitComponent->Update();
        }

        virtual void Draw(Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& context,
            const Rendering::MeshDrawInfo& drawInfo)
            = 0;
    };
} // namespace Beer::System
