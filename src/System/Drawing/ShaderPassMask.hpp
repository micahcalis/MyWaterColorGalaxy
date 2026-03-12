#pragma once

#include "Rendering/Shader/ShaderPassType.hpp"

namespace Beer::System
{
    struct ShaderPassMask
    {
    private:
        uint32_t bits;

    public:
        ShaderPassMask(uint32_t bits)
            : bits(bits)
        {
        }

        ShaderPassMask(Rendering::ShaderPassType pass)
            : bits(static_cast<uint32_t>(pass))
        {
        }

        [[nodiscard]] bool Has(const Rendering::ShaderPassType pass) const
        {
            return (bits & static_cast<uint32_t>(pass)) != 0;
        }
    };
} // namespace Beer::System
