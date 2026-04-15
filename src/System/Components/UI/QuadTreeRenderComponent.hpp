#pragma once

#include "Rendering/Shader/ShaderPassType.hpp"
#include "System/Components/General/IRenderComponent.hpp"
#include "System/Components/UI/UIRenderComponent.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "Rendering/Quads/QuadDrawCallPool.hpp"
#include "Rendering/Quads/QuadBuffer.hpp"

namespace Beer::System
{
    class QuadTreeRenderComponent : public UIRenderComponent
    {
    private:
        Function<std::vector<UIRenderItem>> getRenderItems;
        std::unique_ptr<Rendering::QuadBuffer> quadBuffer;
        std::unique_ptr<Rendering::QuadDrawCallPool> drawCallPool;
        UITransform* rootTransform = nullptr;

    public:
        QuadTreeRenderComponent();

        void SetGetRenderItems(Function<std::vector<UIRenderItem>> getRenderItems)
        {
            this->getRenderItems = getRenderItems;
        }

        void SetRootTransform(UITransform* rootTransform)
        {
            this->rootTransform = rootTransform;
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

        float GetDepth() const override
        {
            if (rootTransform == nullptr)
                return 100.0f;

            return rootTransform->Depth;
        }
    };
} // namespace Beer::System
