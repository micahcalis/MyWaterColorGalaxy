#pragma once

#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Galaxy/General/GalaxyTransformSettings.hpp"
#include "System/Galaxy/General/Objects/IGalaxyObject.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"

namespace Beer::System
{
    class PlanetObject : public IGalaxyObject
    {
    public:
        static constexpr const char* MESH_PATH = "MDL_Cube";
        static constexpr const char* SHADER_PATH = "Galaxy/Planet";

    public:
        PlanetObject(const SerializableGalaxyComponent& serializedData,
            const GalaxyTransformSettings& settings)
            : IGalaxyObject(serializedData, settings, SHADER_PATH)
        {
            material->SetColor("_ColorA", serializedData.Colors[0]);
            material->SetColor("_ColorB", serializedData.Colors[1]);
            material->SetColor("_ColorC", serializedData.Colors[2]);
            material->SetColor("_ColorD", serializedData.Colors[3]);
            material->SetFloat("_Radius", 0.5f); // TEMPORARILY HARDCODED, NEED FIX
        }

        void Draw(Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& context,
            const Rendering::MeshDrawInfo& drawInfo)
            override
        {
            commandBuffer->BindMaterial(material.get());
            commandBuffer->DrawMeshSingle(drawInfo);
        }
    };
} // namespace Beer::System
