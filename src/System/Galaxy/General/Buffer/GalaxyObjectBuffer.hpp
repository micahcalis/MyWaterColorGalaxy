#pragma once

#include "GalaxyDataObject.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Pipeline/CommandBuffer/CommandBuffer.hpp"
#include "Rendering/Pipeline/CommandBuffer/RenderContext.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "Rendering/Shader/ShaderProperty.hpp"
#include "Rendering/Texture/Texture3D.hpp"
#include "System/Galaxy/General/GalaxyObjectType.hpp"
#include "System/Galaxy/General/Buffer/OrbitComponent.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
namespace Beer::System
{
    class GalaxyObjectBuffer
    {
    public:
        static constexpr float GALAXY_POS_SCALE = 1000.0f;
        static constexpr float GALAXY_SIZE_SCALE = 200.0f;

    private:
        std::shared_ptr<Rendering::PhaseBuffer> dataBuffer = nullptr;
        std::shared_ptr<Rendering::PhaseBuffer> positionBuffer;
        std::vector<OrbitComponent> orbitComponents;
        std::vector<glm::vec4> objectPositions;

        std::shared_ptr<Rendering::Material> material = nullptr;
        std::shared_ptr<Rendering::Mesh> mesh = nullptr;
        Rendering::Texture3D* controlNoiseVolume = nullptr;
        uint32_t instanceCount = 0;
        Rendering::ShaderProperty positionBufferProperty{};
        size_t minAligment;
        std::vector<GalaxyDataObject> serializedObjects;

    public:
        GalaxyObjectBuffer(const SerializableGalaxy& serializedData,
            GalaxyObjectType type,
            const char* shaderPath,
            const char* meshPath,
            Rendering::Texture3D* controlNoiseVolume);

        void Update();

        void Draw(Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& context,
            const Rendering::ShaderPassType pass);

    private:
        void InitializeDataBuffer();
        void InitializeDynamicPositions(const SerializableGalaxy& serializedData,
            std::vector<SerializableGalaxyComponent>& components);

        void InitializeMaterialData(const SerializableGalaxy& serializedData);

        float CalculateOrbitSpeed(const glm::vec2 normSunPos, const glm::vec2 normCompPos);
    };
} // namespace Beer::System
