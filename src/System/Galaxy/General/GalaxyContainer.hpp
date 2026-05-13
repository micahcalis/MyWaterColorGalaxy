#pragma once

#include "GalaxyObjectType.hpp"
#include "System/Galaxy/General/Objects/IGalaxyObject.hpp"
#include "Rendering/Mesh/Mesh.hpp"
#include "Rendering/Shader/Shader.hpp"
#include <memory>
#include <type_traits>
#include <unordered_map>

namespace Beer::System
{
    struct GalaxyObjectPool
    {
    public:
        std::vector<std::unique_ptr<IGalaxyObject>> Objects;
        std::shared_ptr<Rendering::Shader> ObjectShader = nullptr;
        std::shared_ptr<Rendering::Mesh> ObjectMesh = nullptr;
    };

    class GalaxyContainer
    {
    private:
        std::unordered_map<GalaxyObjectType, std::unique_ptr<GalaxyObjectPool>> objectMap;

    public:
        template<typename T, typename... Args>
        T* CreateObject(const GalaxyObjectType type, Args&&... args)
        {
            static_assert(std::is_base_of<IGalaxyObject, T>::value,
                "Create Galaxy Object Type does not inherit from IGalaxyObject!");

            std::unique_ptr<T> object = std::make_unique<T>(std::forward<Args>(args)...);
            T* objectP = object.get();

            auto& poolPtr = objectMap[type];

            if (!poolPtr)
            {
                poolPtr = std::make_unique<GalaxyObjectPool>();
            }

            poolPtr->Objects.push_back(std::move(object));

            return objectP;
        }

        void SetPoolShader(const GalaxyObjectType type, std::shared_ptr<Rendering::Shader> shader)
        {
            auto& poolPtr = objectMap[type];

            if (!poolPtr)
            {
                poolPtr = std::make_unique<GalaxyObjectPool>();
            }

            poolPtr->ObjectShader = std::move(shader);
        }

        void SetPoolMesh(const GalaxyObjectType type, std::shared_ptr<Rendering::Mesh> mesh)
        {
            auto& poolPtr = objectMap[type];

            if (!poolPtr)
            {
                poolPtr = std::make_unique<GalaxyObjectPool>();
            }

            poolPtr->ObjectMesh = std::move(mesh);
        }

        [[nodiscard]] GalaxyObjectPool* GetPool(const GalaxyObjectType type) const
        {
            auto it = objectMap.find(type);

            if (it == objectMap.end())
                return nullptr;

            return it->second.get();
        }

        void Clear()
        {
            objectMap.clear();
        }
    };
} // namespace Beer::System
