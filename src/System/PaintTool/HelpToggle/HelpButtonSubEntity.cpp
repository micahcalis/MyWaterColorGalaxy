#include "System/PaintTool/HelpToggle/HelpButtonSubEntity.hpp"
#include "HelpButtonSubEntity.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Text/TextBuffer.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/TextSettings.hpp"
#include "System/Components/UI/TextTransform.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/PaintToolContext.hpp"

namespace Beer::System
{
    static const glm::vec4 FONT_COLOR = glm::vec4(0, 0, 0, 1);
    static const float FONT_SIZE = 0.04f;
    static const float TEXT_PADDING = 0.05f;
    static const float CHARACTER_SPACING = 0.005;
    static const float LINE_SPACING = 1.25f;

    HelpButtonSubEntity::HelpButtonSubEntity(UITransform* rootTransform,
        UITransform buttonTransform,
        const std::string& popupText,
        UITransform popupTransform)
    {
        InitializeButtonFields(rootTransform, buttonTransform);
        InitializePopupFields(popupText, popupTransform);
    }

    std::unique_ptr<Button> HelpButtonSubEntity::CreateHelpButton() const
    {
        return std::make_unique<Button>(buttonEntity->GetTransform(), buttonMaterial.get());
    }

    std::unique_ptr<Button> HelpButtonSubEntity::CreatePopupButton() const
    {
        return std::make_unique<Button>(popupEntity->GetTransform(), popupMaterial.get());
    }

    void HelpButtonSubEntity::SetPopupEnabled(bool enabled)
    {
        popupEntity->GetTransform()->SetEnabled(enabled);
        popupEntity->GetTransform()->SetEnabledInHierarchy(enabled);
    }

    void HelpButtonSubEntity::InitializeButtonFields(UITransform* rootTransform,
        UITransform buttonTransform)
    {
        buttonEntity = std::make_unique<UISubEntity>(buttonTransform);
        rootTransform->BindChild(buttonEntity->GetTransform());

        buttonSprite = std::make_shared<Rendering::Texture2D>("UI/HelpToggle/Tex_HelpIcon");
        buttonMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        buttonMaterial->SetTexture("_SpriteTex", buttonSprite.get());
        buttonMaterial->SetColor("_TintColor", glm::vec4(1));
        buttonMaterial->SetVector("_Scale", glm::vec4(1));
    }

    void HelpButtonSubEntity::InitializePopupFields(const std::string& popupText,
        UITransform popupTransform)
    {
        popupEntity = std::make_unique<UISubEntity>(popupTransform);
        buttonEntity->GetTransform()->BindChild(popupEntity->GetTransform());

        popupSprite = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        popupMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        // popupMaterial->SetTexture("_SpriteTex", popupSprite.get());
        popupMaterial->SetColor("_TintColor", glm::vec4(1));
        popupMaterial->SetVector("_Scale", glm::vec4(1));

        helpTextTransform = std::make_unique<TextTransform>();
        popupEntity->GetTransform()->BindChild(helpTextTransform->GetUITransform());
        helpTextTransform->Anchor = AnchorMode::Center;
        helpTextTransform->Pivot = AnchorMode::Center;
        helpTextTransform->Scale = popupTransform.Scale - TEXT_PADDING;
        helpTextTransform->Depth = popupTransform.Depth + 0.1f;

        helpTextTransform->FontMaterial = std::make_shared<Rendering::FontMaterial>("IndieFlower");
        helpTextTransform->FontMaterial->SetColor(FONT_COLOR);
        helpTextTransform->FontMaterial->SetSize(FONT_SIZE);
        helpTextTransform->TextSettings.CharacterSpacing = CHARACTER_SPACING;
        helpTextTransform->TextSettings.LineSpacing = LINE_SPACING;

        helpTextTransform->TextBuffer = std::make_shared<Rendering::TextBuffer>();

        helpTextTransform->TextBuffer->Update(popupText,
            helpTextTransform->FontMaterial->GetAsset(),
            helpTextTransform->FontMaterial->GetSettings(),
            helpTextTransform->TextSettings,
            helpTextTransform->GetUITransform());
    }
} // namespace Beer::System
