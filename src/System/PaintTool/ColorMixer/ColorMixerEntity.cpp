#include "ColorMixerEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/PaintToolContext.hpp"

namespace Beer::System
{
    ColorMixerEntity::ColorMixerEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        colorMixerDisplayMat = std::make_shared<Rendering::Material>("UI/SpriteDefault");

        rootTransform.Anchor = AnchorMode::Center;
        rootTransform.Pivot = AnchorMode::Center;
        MarkDirty();
    }
} // namespace Beer::System
