#pragma once

#include "System/Drawing/Layer.hpp"

namespace Beer::System
{
    struct LayerMask
    {
    private:
        uint32_t bits;

    public:
        LayerMask(uint32_t bits)
            : bits(bits)
        {
        }

        LayerMask(Layer layer)
            : bits(static_cast<uint32_t>(layer))
        {
        }

        [[nodiscard]] bool Has(const Layer layer) const
        {
            return (bits & static_cast<uint32_t>(layer)) != 0;
        }
    };
} // namespace Beer::System
