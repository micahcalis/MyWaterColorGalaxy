#include "System/PaintTool/HologramCursor/HologramCursorEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include <memory>

namespace Beer::System
{
    HologramCursorEntity::HologramCursorEntity(Function<CursorState> getCursorState,
        Function<MouseInput> getMouseInput,
        Function<Rendering::Texture2D*, GalaxyBrushType> getBrushTexture)
        : getCursorState(getCursorState), getMouseInput(getMouseInput), getBrushTexture(getBrushTexture), QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        rootTransform.Anchor = AnchorMode::BottomLeft;
        rootTransform.Pivot = AnchorMode::Center;
        rootTransform.Scale = glm::vec2(0.05f, 0.05f);

        cursorMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        cursorMaterial->SetVector("_Scale", glm::vec4(1));

        MarkDirty();
    }
} // namespace Beer::System
