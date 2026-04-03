#pragma once

#include "System/Components/General/Transform.hpp"
#include "System/Components/Registry/GameSubEntity.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Delegates/Delegate.hpp"
#include <vector>

namespace Beer::System
{
    class MultipleContainerManager : public IEntityManager
    {
    protected:
        Transform* parent;
        Function<std::vector<GameSubEntity>&> getSubEntities;

    public:
        virtual ~MultipleContainerManager() = default;

        void SetParent(Transform* parent)
        {
            this->parent = parent;
        }

        void SetGetSubEntitiesFunction(Function<std::vector<GameSubEntity>&> getSubEntities)
        {
            this->getSubEntities = getSubEntities;
        }

        void Update() override
        {
            if (getSubEntities == nullptr)
                return;

            std::vector<GameSubEntity>& subEntities = getSubEntities();
            uint32_t index = 0;
            for (auto& subEntity : subEntities)
            {
                UpdateSubEntity(subEntity, index, subEntities.size());
                index++;
            }
        }

    protected:
        virtual void UpdateSubEntity(GameSubEntity& subEntity, uint32_t index, uint32_t entityCount) = 0;
    };
} // namespace Beer::System
