#pragma once

#include "NebulaBuffer.hpp"
#include "Rendering/RenderPasses/Watercolor/NebulaBuffer.hpp"
#include "System/Components/General/ISubRenderPipeline.hpp"
#include "System/Galaxy/General/Buffer/GalaxyObjectBuffer.hpp"

namespace Beer::System
{
    class NebulaSubPipeline : public ISubRenderPipeline
    {
    private:
        GalaxyObjectBuffer* stardustBuffer = nullptr;
        std::unique_ptr<Rendering::NebulaBuffer> nebulaBuffer = nullptr;

    public:
        NebulaSubPipeline(GalaxyObjectBuffer* stardustBuffer)
            : stardustBuffer(stardustBuffer)
        {
            nebulaBuffer = std::make_unique<Rendering::NebulaBuffer>();
        }
    };
} // namespace Beer::System
