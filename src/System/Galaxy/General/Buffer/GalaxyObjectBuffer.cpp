#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"
#include "GalaxyBufferSettings.hpp"
#include "GalaxyDataObject.hpp"
#include "Rendering/Buffer/Buffer.hpp"
#include "Rendering/Buffer/PhaseBuffer.hpp"
#include "Rendering/Buffer/SSBOType.hpp"
#include "Rendering/Material/Material.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "System/Galaxy/General/GalaxyObjectType.hpp"
#include "System/Galaxy/General/Buffer/OrbitComponent.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"

namespace Beer::System
{
    GalaxyObjectBuffer::GalaxyObjectBuffer(const SerializableGalaxy& serializedData,
        GalaxyObjectType type,
        const char* shaderPath,
        const char* meshPath,
        Rendering::Texture3D* controlNoiseVolume,
        std::shared_ptr<IGalaxyBufferSettings> settings)
        : settings(settings)
        , controlNoiseVolume(controlNoiseVolume)
    {
        material = std::make_shared<Rendering::Material>(shaderPath);
        settings->ApplyMaterialSettings(material.get());
        material->SetTexture("_ControlNoiseVolume", controlNoiseVolume);

        mesh = Rendering::Mesh::Get(meshPath);

        std::vector<SerializableGalaxyComponent> components;
        serializedObjects.reserve(serializedData.Components.size());
        components.reserve(serializedData.Components.size());

        for (const auto& component : serializedData.Components)
        {
            bool isType = type == static_cast<GalaxyObjectType>(component.TypeIndex);

            if (isType)
            {
                serializedObjects.push_back({component.Colors[0],
                    component.Colors[1],
                    component.Colors[2],
                    component.Colors[3],
                    component.Scale * GALAXY_SIZE_SCALE,
                    component.Id});

                components.push_back(component);
            }
        }

        instanceCount = serializedObjects.size();

        if (instanceCount == 0)
            return;

        InitializeDataBuffer();
        InitializeDynamicPositions(serializedData, components);
        InitializeMaterialData(serializedData);
    }

    void GalaxyObjectBuffer::Update()
    {
        if (instanceCount == 0)
            return;

        for (size_t i = 0; i < orbitComponents.size(); i++)
        {
            orbitComponents[i].Update(objectPositions[i]);
        }

        size_t frameIndex = Rendering::UniformDescriptor::GetFrameIndex();
        size_t rawSize = sizeof(glm::vec4) * objectPositions.size();
        size_t alignedChunkSize = (rawSize + minAligment - 1) & ~(minAligment - 1);
        size_t offset = alignedChunkSize * frameIndex;

        positionBuffer->GetHandle()->Upload(objectPositions.data(), rawSize, offset);
    }

    void GalaxyObjectBuffer::Draw(Rendering::CommandBuffer* commandBuffer,
        const Rendering::RenderContext& context,
        const Rendering::ShaderPassType pass)
    {
        if (instanceCount == 0)
            return;

        const Rendering::Shader* shader = material->GetShader();

        if (!shader->HasPass(pass))
            return;

        const Rendering::ShaderPass* shaderPass = shader->GetPass(pass);

        commandBuffer->BindShaderPass(shader, shaderPass, context.Output);

        size_t frameIndex = Rendering::UniformDescriptor::GetFrameIndex();
        size_t rawSize = sizeof(glm::vec4) * objectPositions.size();
        size_t alignedChunkSize = (rawSize + minAligment - 1) & ~(minAligment - 1);
        uint32_t offset = static_cast<uint32_t>(alignedChunkSize * frameIndex);

        commandBuffer->BindMaterial(material.get(), {offset});
        commandBuffer->BindMesh(mesh.get(), &shaderPass->Input.BufferOrder);

        Rendering::MeshDrawInfo drawInfo = mesh->GetDrawInfo();
        commandBuffer->DrawMeshMultiple(drawInfo, instanceCount);
    }

    void GalaxyObjectBuffer::InitializeDataBuffer()
    {
        size_t size = sizeof(GalaxyDataObject) * serializedObjects.size();

        dataBuffer = std::make_shared<Rendering::PhaseBuffer>("_",
            std::make_shared<Rendering::Buffer>(Rendering::Buffer::CreateSSBO(size, Rendering::SSBOType::Consistent)));

        dataBuffer->UploadAsync(serializedObjects.data(), size);
    }

    void GalaxyObjectBuffer::InitializeDynamicPositions(const SerializableGalaxy& serializedData,
        std::vector<SerializableGalaxyComponent>& components)
    {
        minAligment = Rendering::UniformDescriptor::GetMinAlignment(Rendering::BufferDescriptorType::Storage);

        size_t rawSize = sizeof(glm::vec4) * instanceCount;
        size_t alignedChunkSize = (rawSize + minAligment - 1) & ~(minAligment - 1);
        size_t totalBufferSize = alignedChunkSize * Rendering::UniformDescriptor::GetFramesInFlight();

        positionBuffer = std::make_shared<Rendering::PhaseBuffer>("_",
            std::make_shared<Rendering::Buffer>(Rendering::Buffer::CreateDynamic(totalBufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT)));

        orbitComponents.reserve(components.size());
        objectPositions.resize(instanceCount);
        glm::vec3 sunCenter = glm::vec3(serializedData.StarPosition.x, 0, serializedData.StarPosition.y) * GALAXY_POS_SCALE;

        for (const auto& component : components)
        {
            float speed = CalculateOrbitSpeed(serializedData.StarPosition, component.Position);

            orbitComponents.push_back(OrbitComponent(speed,
                OrbitDirection::ClockWise,
                sunCenter,
                serializedData.OrbitShear,
                component.Tilt,
                glm::vec3(component.Position.x, 0, component.Position.y) * GALAXY_POS_SCALE));
        }
    }

    void GalaxyObjectBuffer::InitializeMaterialData(const SerializableGalaxy& serializedData)
    {
        material->SetStructuredBuffer("_GalaxyObjectData", dataBuffer.get());
        material->SetStructuredBuffer("_DynamicGalaxyObjectPositions", positionBuffer.get());
    }

    float GalaxyObjectBuffer::CalculateOrbitSpeed(const glm::vec2 normSunPos, const glm::vec2 normCompPos)
    {
        float dx = normCompPos.x - normSunPos.x;
        float dy = normCompPos.y - normSunPos.y;
        float radius = std::sqrt(dx * dx + dy * dy);

        return settings->SpeedMultiplier / (std::sqrt(radius) + 1.0f);
    }
} // namespace Beer::System
