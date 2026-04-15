#include "System/Components/Registry/IEntity.hpp"

namespace Beer::System
{
    void IEntity::SetId(Registry* assigner, uint32_t id)
    {
        if (assigner == nullptr)
            return;

        this->id = id;
    }

    bool IEntity::IsAssigned() const
    {
        return id != 0;
    }
} // namespace Beer::System
