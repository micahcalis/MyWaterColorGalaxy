#include "System/Galaxy/General/GalaxyContainer.hpp"
#include "Buffer/GalaxyObjectBuffer.hpp"
#include "System/Galaxy/General/GalaxyObjectType.hpp"

namespace Beer::System
{
    static const std::array<GalaxyBufferDefinition, 5> DEFINITIONS = {
        GalaxyBufferDefinition(GalaxyObjectType::Planet,
            "Galaxy/Planet",
            "MDL_Cube"),
        GalaxyBufferDefinition(GalaxyObjectType::Asteroids,
            "Galaxy/Planet",
            "MDL_Cube"),
        GalaxyBufferDefinition(GalaxyObjectType::SpaceGoo,
            "Galaxy/Planet",
            "MDL_Cube"),
        GalaxyBufferDefinition(GalaxyObjectType::BlackHole,
            "Galaxy/Planet",
            "MDL_Cube"),
        GalaxyBufferDefinition(GalaxyObjectType::StarDust,
            "Galaxy/Planet",
            "MDL_Cube")};

    void GalaxyContainer::CreateBuffers(const SerializableGalaxy& serializedData)
    {
        for (const auto& definition : DEFINITIONS)
        {
            bufferMap[definition.Type] = std::make_unique<GalaxyObjectBuffer>(serializedData,
                definition.Type,
                definition.ShaderPath,
                definition.MeshPath);
        }
    }

    void GalaxyContainer::Draw(Rendering::CommandBuffer* commandBuffer,
        const Rendering::RenderContext& renderContext,
        const Rendering::ShaderPassType pass)
    {
        for (const auto& bufferPair : bufferMap)
        {
            bufferPair.second->Draw(commandBuffer,
                renderContext,
                pass);
        }
    }

    void GalaxyContainer::Update()
    {
        for (const auto& bufferPair : bufferMap)
        {
            bufferPair.second->Update();
        }
    }
} // namespace Beer::System
