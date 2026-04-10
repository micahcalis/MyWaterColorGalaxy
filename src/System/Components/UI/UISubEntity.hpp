#pragma once

#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class UISubEntity
    {
    private:
        UITransform transform{};

    public:
        UISubEntity(UITransform transform)
            : transform(transform)
        {
        }

        [[nodiscard]] UITransform* GetTransform() { return &transform; }
    };
} // namespace Beer::System
