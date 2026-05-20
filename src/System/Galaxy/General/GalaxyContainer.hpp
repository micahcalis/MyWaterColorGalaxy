#pragma once

#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"
#include "GalaxyObjectType.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include <memory>
#include <type_traits>
#include <unordered_map>

namespace Beer::System
{
    struct GalaxyBufferDefinition
    {
    public:
        const GalaxyObjectType Type;
        const char* ShaderPath;
        const char* MeshPath;

    public:
        GalaxyBufferDefinition(const GalaxyObjectType type,
            const char* shaderPath,
            const char* meshPath)
            : Type(type), ShaderPath(shaderPath), MeshPath(meshPath)
        {
        }
    };

    class GalaxyContainer
    {
    private:
        std::unordered_map<GalaxyObjectType, std::unique_ptr<GalaxyObjectBuffer>> bufferMap;

    public:
        void CreateBuffers(const SerializableGalaxy& serializedData);
        void Update();

        void Draw(Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            const Rendering::ShaderPassType pass);

        void Clear()
        {
            bufferMap.clear();
        }
    };
} // namespace Beer::System
