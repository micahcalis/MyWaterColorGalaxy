#pragma once

#include "System/Components/General/Transform.hpp"

namespace Beer::System
{
    class GameSubEntity
    {
    private:
        Transform transform{};

    public:
        GameSubEntity(Transform transform)
            : transform(transform)
        {
        }

        [[nodiscard]] Transform* GetTransform() { return &transform; }
    };
} // namespace Beer::System
