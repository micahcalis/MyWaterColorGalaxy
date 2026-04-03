#pragma once

#include "MulitpleContainerManager.hpp"
#include "Rendering/Shader/Globals/ModelTransformData.hpp"
#include "System/Components/General/MultipleMeshRender.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/Registry/GameEntity.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Drawing/Layer.hpp"
#include <memory>

namespace Beer::System
{
    template<typename T>
    class MultipleContainerEntity : public GameEntity
    {
        static_assert(std::is_base_of_v<MultipleContainerManager, T>, "Requested type must inherit from MultipleContainerManager!");

    private:
        std::vector<GameSubEntity> subEntities;

    public:
        template<typename... Args>
        MultipleContainerEntity(Transform transform,
            std::unique_ptr<MultipleMeshRender> multipleMeshRender,
            uint32_t subEntityCount,
            Layer layer,
            Args&&... args)
            : GameEntity(transform, nullptr, layer)
        {
            subEntities.reserve(subEntityCount);

            for (uint32_t i = 0; i < subEntityCount; i++)
            {
                subEntities.push_back(GameSubEntity(Transform()));
            }

            multipleMeshRender->SetModelDataFunction([this]() { return GetModelTransformData(); });
            multipleMeshRender->SetLayer(&this->layer);
            renderComponent = std::move(multipleMeshRender);

            manager = std::make_unique<T>(std::forward<Args>(args)...);
        }

        void Update() override
        {
            if (manager == nullptr)
                return;

            manager->Update();
        }

    private:
        void InitializeManager() override
        {
            MultipleContainerManager* multipleManager = static_cast<MultipleContainerManager*>(manager.get());
            multipleManager->SetParent(&transform);

            Function<std::vector<GameSubEntity>&> getSubEntities = [this]() -> std::vector<GameSubEntity>& {
                return subEntities;
            };

            multipleManager->SetGetSubEntitiesFunction(getSubEntities);
        }

        std::vector<Rendering::ModelTransformData> GetModelTransformData()
        {
            std::vector<Rendering::ModelTransformData> data;

            data.reserve(subEntities.size());

            for (auto& subEntity : subEntities)
            {
                const Transform* transform = subEntity.GetTransform();
                data.push_back(transform->GetModelTransformData());
            }

            return data;
        }
    };
} // namespace Beer::System
