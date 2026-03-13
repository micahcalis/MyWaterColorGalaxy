#pragma once

#include "System/Drawing/BindType.hpp"

namespace Beer::System
{
    struct BindMask
    {
    private:
        uint32_t bits;

    public:
        BindMask(uint32_t bits)
            : bits(bits)
        {
        }

        BindMask(BindType layer)
            : bits(static_cast<uint32_t>(layer))
        {
        }

        [[nodiscard]] bool Has(const BindType layer) const
        {
            return (bits & static_cast<uint32_t>(layer)) != 0;
        }
    };
} // namespace Beer::System
