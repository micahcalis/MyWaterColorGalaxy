#pragma once

#include "Rendering/Pipeline/Frame/Dependency/ResourceAction.hpp"

namespace Beer::Rendering
{
    struct ResourceActionMask
    {
    private:
        uint32_t bits;

    public:
        ResourceActionMask(uint32_t bits)
            : bits(bits)
        {
        }

        ResourceActionMask(ResourceAction action)
            : bits(static_cast<uint32_t>(action))
        {
        }

        [[nodiscard]] bool Has(const ResourceAction action) const
        {
            return (bits & static_cast<uint32_t>(action)) != 0;
        }
    };
} // namespace Beer::Rendering
