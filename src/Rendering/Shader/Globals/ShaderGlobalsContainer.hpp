#pragma once

#include "Rendering/Shader/Globals/EngineGlobals.hpp"
#include <memory>
#include <vector>
#include "Rendering/Buffer/Buffer.hpp"

namespace Beer::Rendering
{
    class ShaderGlobalsContainer
    {
    private:
        std::vector<EngineGlobals> engineGlobals;
        std::unique_ptr<Buffer> engineBuffer;

    public:
        ShaderGlobalsContainer(uint32_t framesInFlight)
        {
            engineGlobals = std::vector<EngineGlobals>(framesInFlight);
        }

        EngineGlobals* GetEngine(uint32_t frameIndex)
        {
            return &engineGlobals[frameIndex];
        }
    };
} // namespace Beer::Rendering
