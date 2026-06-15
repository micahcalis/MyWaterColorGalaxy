#include "System/PaintTool/ModeButton/ModeButtonEntity.hpp"
#include "ModeButtonEntity.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"

namespace Beer::System
{
    static const glm::vec2 BUTTON_SIZE = glm::vec2(0.3f, 0.3f);
    static const glm::vec2 BUTTON_OFFSET = glm::vec2(0.05f, 0.05f);

    ModeButtonEntity::ModeButtonEntity(Function<void, bool> setColorModeActive,
        Function<void, bool> setBrushModeActive,
        Function<ButtonInput> getTabKeyInput)
        : setColorModeActive(setColorModeActive)
        , setBrushModeActive(setBrushModeActive)
        , getTabKeyInput(getTabKeyInput)
        , QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        modeButtonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        modeButtonMaterial->SetColor("_TintColor", glm::vec4(1));
        modeButtonMaterial->SetVector("_Scale", glm::vec4(1));

        colorModeTexture = std::make_shared<Rendering::Texture2D>("UI/ModeButton/Tex_ColorMode");
        brushModeTexture = std::make_shared<Rendering::Texture2D>("UI/ModeButton/Tex_BrushMode");

        rootTransform.Anchor = AnchorMode::BottomLeft;
        rootTransform.Pivot = AnchorMode::BottomLeft;
        rootTransform.Scale = BUTTON_SIZE;
        rootTransform.Position = BUTTON_OFFSET;
    }

    void ModeButtonEntity::InitializeModeButton(BrushMenuMode menuMode)
    {
        GetModeButtonManager()->InitializeModeButton(&rootTransform,
            modeButtonMaterial.get(),
            colorModeTexture.get(),
            brushModeTexture.get(),
            setColorModeActive,
            setBrushModeActive,
            menuMode);
    }
} // namespace Beer::System
