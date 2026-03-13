#include "System/Components/Registry/GameEntity.hpp"
#include "GameEntity.hpp"

namespace Beer::System
{
    void GameEntity::SetId(Registry* assigner, uint32_t id)
    {
        if (assigner == nullptr)
            return;

        this->id = id;
    }

    bool GameEntity::IsAssigned() const
    {
        return id != 0;
    }
} // namespace Beer::System
