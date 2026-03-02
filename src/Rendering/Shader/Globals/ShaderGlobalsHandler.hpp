#pragma once

#include "ShaderGlobalsContainer.hpp"
#include <memory>
#include "Core/Application/Renderer/Device.hpp"

namespace Beer::Rendering
{
    class ShaderGlobalsHandler
    {
    private:
        // std::unique_ptr<ShaderGlobalsContainer> container = nullptr;

    public:
        ShaderGlobalsHandler(const Core::Device* device, uint32_t framesInFlight);
        //  ShaderGlobalsContainer* GetContainer() { return container.get(); }
        void Update();
    };
} // namespace Beer::Rendering
