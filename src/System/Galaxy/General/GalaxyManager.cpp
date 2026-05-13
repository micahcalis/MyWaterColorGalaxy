#include "System/Galaxy/General/GalaxyManager.hpp"
#include "GalaxyManager.hpp"
#include "GalaxyObjectType.hpp"
#include "GalaxyTransformSettings.hpp"
#include "Objects/PlanetObject.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Galaxy/General/GalaxyContainer.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include "Vendor/magic_enum/magic_enum.hpp"
#include <memory>
#include <print>

namespace Beer::System
{
    static const float GALAXY_POS_SCALE = 1000.0f;
    static const float GALAXY_SIZE_SCALE = 200.0f;
    static const float GALAXY_TILT = 0.5f;

    void GalaxyManager::DefineTypes()
    {
        RegisterType<PlanetObject>(GalaxyObjectType::Planet);
    }

    void GalaxyManager::InitializePools()
    {
        std::shared_ptr<Rendering::Shader> shader;
        std::shared_ptr<Rendering::Mesh> mesh;

        for (const auto& [type, definition] : typeDefintions)
        {
            shader = Rendering::Shader::Get(definition.ShaderPath);
            mesh = Rendering::Mesh::Get(definition.MeshPath);

            container->SetPoolShader(type, std::move(shader));
            container->SetPoolMesh(type, std::move(mesh));
        }
    }

    void GalaxyManager::Load(const SerializableGalaxy& serializedGalaxy)
    {
        ApplyGalaxySettings(serializedGalaxy);

        for (const auto& component : serializedGalaxy.Components)
        {
            GalaxyObjectType type = static_cast<GalaxyObjectType>(component.TypeIndex);

            auto it = typeDefintions.find(type);

            if (it == typeDefintions.end())
                continue;

            it->second.CreateObject(component);
            // std::println("Object Created of type: {}", magic_enum::enum_name(type));
        }
    }

    void GalaxyManager::Clear()
    {
        container->Clear();
    }

    void GalaxyManager::ApplyGalaxySettings(const SerializableGalaxy& serializedGalaxy)
    {
        settings.ColorSeed = serializedGalaxy.ColorSeed;
        settings.StarSeed = serializedGalaxy.StarSeed;
        settings.SunCenter = glm::vec3(serializedGalaxy.StarPosition.x, 0, serializedGalaxy.StarPosition.y) * GALAXY_POS_SCALE;
        settings.PositionScale = GALAXY_POS_SCALE;
        settings.SizeScale = GALAXY_SIZE_SCALE;
        settings.TiltIntensity = GALAXY_TILT;
    }

    void GalaxyManager::UpdateOrbits()
    {
        return;

        for (uint32_t i = 0; i < NUM_GALAXY_OBJ_TYPES; i++)
        {
            GalaxyObjectPool* pool = container->GetPool(static_cast<GalaxyObjectType>(i));

            if (pool != nullptr)
            {
                for (const auto& object : pool->Objects)
                {
                    object->Update();
                }
            }
        }
    }

} // namespace Beer::System
