#include "System/PaintTool/TransitionMaterialGetter.hpp"
#include "Rendering/Compute/ComputeContext.hpp"
#include "Rendering/RenderPasses/FullscreenTransitionPass.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Texture/TextureMakeSettings.hpp"
#include "glm/fwd.hpp"
#include <memory>

namespace Beer::System
{
    static const float HYPER_SCALE = 0.5f;
    static const glm::vec4 LIGHT_COLOR = glm::vec4(0.775f, 0.98, 1.0f, 1.0f);
    static const glm::vec4 DARK_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    static const uint32_t NOISE_RESOLUTION = 2048;
    static const uint32_t NOISE_DEPTH = 3;
    static const uint32_t NOISE_SEED = 832;
    static const VkFormat NOISE_FORMAT = VK_FORMAT_R8_UNORM;
    static const uint32_t NOISE_KERNEL = 0;
    static const uint32_t NOISE_GROUP_SIZE = 8;
    static const float NOISE_FREQ = 10.0f;
    static const float NOISE_FREQ_MUL = 1.25f;
    static const float NOISE_AMP_MUL = 0.9f;
    static const float NOISE_EXP = 1.0f;
    static const float NOISE_EDGE = 2.0f;

    Rendering::TransitionInitialization TransitionMaterialGetter::GetHyperspaceInitialization()
    {
        auto initialization = [](std::shared_ptr<Rendering::ComputeContext>& context, std::shared_ptr<Rendering::Texture2D>& texture)
            -> std::shared_ptr<Rendering::Material> {
            auto transitionMaterial = std::make_shared<Rendering::Material>("Blit/SpaceTransitionBlit");
            transitionMaterial->SetFloat("_EffectScale", HYPER_SCALE);
            transitionMaterial->SetColor("_LightColor", LIGHT_COLOR);
            transitionMaterial->SetColor("_DarkColor", DARK_COLOR);

            Rendering::TextureMakeSettings makeSettings{};
            makeSettings.Width = NOISE_RESOLUTION;
            makeSettings.Height = NOISE_RESOLUTION;
            makeSettings.Format = NOISE_FORMAT;
            makeSettings.GroupSizeX = NOISE_GROUP_SIZE;
            makeSettings.GroupSizeY = NOISE_GROUP_SIZE;
            makeSettings.KernelIndex = NOISE_KERNEL;

            context = std::make_shared<Rendering::ComputeContext>("Painting/TransitionNoise");
            context->SetInt("_Depth", NOISE_DEPTH);
            context->SetInt("_Seed", NOISE_SEED);
            context->SetFloat("_Frequency", NOISE_FREQ);
            context->SetFloat("_FrequencyMultiplier", NOISE_FREQ_MUL);
            context->SetFloat("_AmplitudeMultiplier", NOISE_AMP_MUL);
            context->SetFloat("_Exponent", NOISE_EXP);
            context->SetFloat("_Edge", NOISE_EDGE);

            texture = std::make_shared<Rendering::Texture2D>(Rendering::Texture2D::Make(makeSettings,
                context.get()));

            transitionMaterial->SetTexture("_NoiseTex", texture.get());

            return transitionMaterial;
        };

        return initialization;
    }
} // namespace Beer::System
