#pragma once

#include "GalaxyObjectType.hpp"
#include "GalaxyTransformSettings.hpp"
#include "Objects/IGalaxyObject.hpp"
#include "System/Galaxy/General/GalaxyTransformSettings.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySeed.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
#include "System/Galaxy/General/GalaxyContainer.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include <concepts>

namespace Beer::System
{
    template<typename T>
    concept ValidGalaxyObjectDef = requires {
        { T::MESH_PATH } -> std::convertible_to<const char*>;
        { T::SHADER_PATH } -> std::convertible_to<const char*>;
        requires std::derived_from<T, IGalaxyObject>;
        requires std::constructible_from<T, const SerializableGalaxyComponent&, const GalaxyTransformSettings&>;
    };

    struct GalaxyTypeDefinition
    {
        std::string ShaderPath;
        std::string MeshPath;
        Function<IGalaxyObject*, const SerializableGalaxyComponent&> CreateObject = nullptr;
    };

    class GalaxyManager : public IEntityManager
    {
    private:
        GalaxyContainer* container = nullptr;
        GalaxyTransformSettings settings{};
        std::unordered_map<GalaxyObjectType, GalaxyTypeDefinition> typeDefintions;

    public:
        GalaxyManager(GalaxyContainer* container)
            : container(container)
        {
            DefineTypes();
            InitializePools();
        }

        void Update() override
        {
            UpdateOrbits();
        }

        void Load(const SerializableGalaxy& serializedGalaxy);
        void Clear();

    private:
        template<ValidGalaxyObjectDef T>
        void RegisterType(GalaxyObjectType type)
        {
            Function<IGalaxyObject*, const SerializableGalaxyComponent&> createObject = [this, type](const SerializableGalaxyComponent& component) -> IGalaxyObject* {
                return this->container->CreateObject<T>(type, component, settings);
            };

            typeDefintions[type] = {
                T::SHADER_PATH,
                T::MESH_PATH,
                createObject};
        }

        void DefineTypes();
        void InitializePools();
        void ApplyGalaxySettings(const SerializableGalaxy& serializedGalaxy);
        void UpdateOrbits();
    };
} // namespace Beer::System
