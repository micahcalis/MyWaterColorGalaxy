#pragma once

#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "Rendering/RenderPasses/Painting/InteractivePaintingPass.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/PaintTool/ColorMixer/ColorMixerEntity.hpp"
#include "System/Context/IContext.hpp"

namespace Beer::System
{
    class PaintToolContext : public IContext
    {
    private:
        ColorMixerEntity* colorMixerEntity = nullptr;
        Rendering::InteractivePaintingPass* interactivePaintingPass = nullptr;
        Rendering::DrawUIPass* drawUIPass = nullptr;
        Function<MouseInput> getMouseInput;

    public:
        PaintToolContext(Function<MouseInput> getMouseInput)
            : getMouseInput(getMouseInput)
        {
        }

        void Load() override;
        void Update() override;
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;
    };
} // namespace Beer::System
