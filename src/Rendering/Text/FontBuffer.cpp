#include "Rendering/Text/FontBuffer.hpp"
#include "FontBuffer.hpp"
#include "Rendering/Text/FontSettings.hpp"
#include "Rendering/Texture/RenderTexture.hpp"
#include "Rendering/Uniforms/UniformDescriptor.hpp"
#include "vulkan/vulkan.hpp"
#include <memory>

namespace Beer::Rendering
{
    const uint32_t FONT_MAT_BINDING = 0;
    const uint32_t FONT_ATLAS_BINDING = 1;

    FontBuffer::FontBuffer(Texture2D* atlasTexture)
        : fontBinding(sizeof(FontSettings), FONT_MAT_BINDING, 1)
    {
        auto bindings = GetBindings();
        descriptor = std::make_unique<UniformDescriptor>(bindings);

        for (uint32_t i = 0; i < UniformDescriptor::GetFramesInFlight(); i++)
        {
            buffers.push_back(std::make_shared<Buffer>(
                Buffer::CreateUniform(sizeof(FontSettings))));

            descriptor->UpdateBufferInfo(i,
                FONT_MAT_BINDING,
                *buffers[i],
                sizeof(FontSettings));

            descriptor->UpdateImageInfo(i,
                FONT_ATLAS_BINDING,
                TextureAccess::Standard,
                atlasTexture);
        }
    }

    void FontBuffer::Update(const FontSettings& fontSettings)
    {
        if (!buffers.empty())
        {
            buffers[UniformDescriptor::GetFrameIndex()]->Upload(&fontSettings, sizeof(FontSettings));
        }
    }

    std::vector<vk::DescriptorSetLayoutBinding> FontBuffer::GetBindings()
    {
        return {
            vk::DescriptorSetLayoutBinding(
                FONT_MAT_BINDING,
                vk::DescriptorType::eUniformBuffer,
                1,
                vk::ShaderStageFlagBits::eAllGraphics),

            vk::DescriptorSetLayoutBinding(
                FONT_ATLAS_BINDING,
                vk::DescriptorType::eCombinedImageSampler,
                1,
                vk::ShaderStageFlagBits::eAllGraphics)};
    }
} // namespace Beer::Rendering
