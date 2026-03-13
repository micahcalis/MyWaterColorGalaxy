#pragma once

#include "System/Context/ContextType.hpp"

namespace Beer::System
{
    struct ContextMask
    {
    private:
        uint32_t bits;

    public:
        ContextMask(uint32_t bits)
            : bits(bits)
        {
        }

        ContextMask(ContextType context)
            : bits(static_cast<uint32_t>(context))
        {
        }

        [[nodiscard]] bool Has(const ContextType context) const
        {
            return (bits & static_cast<uint32_t>(context)) != 0;
        }
    };
} // namespace Beer::System
