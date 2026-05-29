#include "Rendering/RenderPasses/Watercolor/BlitMainColorPass.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Pipeline/Frame/Dependency/PassDependencyList.hpp"
#include "Rendering/Pipeline/IRenderPass.hpp"
#include "Rendering/Shader/ShaderPassType.hpp"

namespace Beer::Rendering
{
    BlitMainColorPass::BlitMainColorPass(const std::string& destinationName,
        FullscreenResolution destinationResolution,
        RenderPassEvent event,
        int32_t eventOffset)
        : destinationName(destinationName)
        , destinationResolution(destinationResolution)
        , IRenderPass("BlitMainColorPass", static_cast<int32_t>(event) + eventOffset)
    {
        blitMaterial = std::make_shared<Rendering::Material>("Blit/BlitColor");
    }

    void BlitMainColorPass::OnRenderSetup(const RenderContext& context)
    {
        uint32_t resolutionX = (uint32_t)((float)Core::Screen::Width() / (float)destinationResolution);
        uint32_t resolutionY = (uint32_t)((float)Core::Screen::Height() / (float)destinationResolution);

        context.BlackBox->ReallocateIfNeeded(destinationName,
            resolutionX,
            resolutionY,
            Core::Screen::ColorFormat());
    }

    void BlitMainColorPass::Execute(CommandBuffer* commandBuffer, const RenderContext& context)
    {
        commandBuffer->Blit(context.GetMainColorDestination(),
            blitMaterial.get(),
            ShaderPassType::BlitColor,
            context.Output);
    }

    PassDependencyList BlitMainColorPass::GetDependencies() const
    {
        PassDependencyList dependencies = PassDependencyList(name);

        dependencies.AddDependency(PassDependency(std::string(VIRTUAL_MAIN_COLOR),
            ResourceAction::ColorRead,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        dependencies.AddDependency(PassDependency(destinationName,
            ResourceAction::ColorWrite,
            ResetOperator::ClearColor({0.0f, 0.0f, 0.0f, 0.0f}),
            static_cast<vk::Format>(Core::Screen::ColorFormat())));

        return dependencies;
    }
} // namespace Beer::Rendering
