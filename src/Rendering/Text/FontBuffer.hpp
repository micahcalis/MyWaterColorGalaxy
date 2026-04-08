#pragma once

#include "Rendering/Shader/Globals/GlobalBuffer.hpp"
#include "Rendering/Text/FontSettings.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "Rendering/Uniforms/IShaderResource.hpp"

namespace Beer::Rendering
{
    class FontBuffer : public IShaderResource
    {
        BufferBinding fontBinding;

    public:
        FontBuffer(Texture2D* atlasTexture);
        void Update(const FontSettings& fontSettings);

    protected:
        std::vector<vk::DescriptorSetLayoutBinding> GetBindings() override;
    };
} // namespace Beer::Rendering
