#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Text/FontMaterial.hpp"
#include "Rendering/Text/TextBuffer.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Components/UI/TextTransform.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::System
{
    class HelpButtonSubEntity
    {
    private:
        std::unique_ptr<UISubEntity> buttonEntity = nullptr;
        std::shared_ptr<Rendering::Material> buttonMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> buttonSprite = nullptr;

        std::unique_ptr<UISubEntity> popupEntity = nullptr;
        std::shared_ptr<Rendering::Material> popupMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> popupSprite = nullptr;

        std::unique_ptr<System::TextTransform> helpTextTransform = nullptr;

    public:
        HelpButtonSubEntity(UITransform* rootTransform,
            UITransform buttonTransform,
            const std::string& popupText,
            UITransform popupTransform);

        std::unique_ptr<Button> CreateHelpButton() const;
        std::unique_ptr<Button> CreatePopupButton() const;

        void SetPopupEnabled(bool enabled);

        std::vector<UIRenderItem> GetRenderItems()
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.reserve(3);

            renderItems.push_back(UIRenderItem(buttonEntity->GetTransform(), buttonMaterial.get()));
            renderItems.push_back(UIRenderItem(popupEntity->GetTransform(), popupMaterial.get()));

            renderItems.push_back(UIRenderItem(helpTextTransform->GetUITransform(),
                helpTextTransform->FontMaterial.get(),
                helpTextTransform->TextBuffer.get()));

            return renderItems;
        }

    private:
        void InitializeButtonFields(UITransform* rootTransform,
            UITransform buttonTransform);

        void InitializePopupFields(const std::string& popupText,
            UITransform popupTransform);
    };

} // namespace Beer::System
