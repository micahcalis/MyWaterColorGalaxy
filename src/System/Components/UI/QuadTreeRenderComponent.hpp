#pragma once

#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "Rendering/Quads/QuadDrawCallPool.hpp"
#include "Rendering/Quads/QuadBuffer.hpp"

namespace Beer::System
{
    class QuadTreeRenderComponent : public IRenderComponent
    {
    private:
        Function<std::vector<UIRenderItem>> getRenderItems;
        std::unique_ptr<Rendering::QuadBuffer> quadBuffer;
        std::unique_ptr<Rendering::QuadDrawCallPool> drawCallPool;

    public:
        QuadTreeRenderComponent();

        void SetGetRenderItems(Function<std::vector<UIRenderItem>> getRenderItems)
        {
            this->getRenderItems = getRenderItems;
        }

        void UpdateQuadDraw();

        BindHistory Bind(BindMask mask,
            Rendering::CommandBuffer* commandBuffer,
            const Rendering::RenderContext& renderContext,
            const Rendering::ShaderPassType pass) override;

        bool HasPass(Rendering::ShaderPassType pass) const override { return pass == Rendering::ShaderPassType::UserInterface; }
        Layer GetLayer() const override { return Layer::UI; }
        BindMask GetBindMask(const BindHistory& history) const override;

        const Rendering::Shader* GetPrimaryShader() const override { return nullptr; }
        const Rendering::Material* GetPrimaryMaterial() const override { return nullptr; }
        const Rendering::Mesh* GetPrimaryMesh() const override { return nullptr; }
    };
} // namespace Beer::System
