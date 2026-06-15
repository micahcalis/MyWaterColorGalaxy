#pragma once

#include "HelpButtonSubEntity.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/PaintTool/HelpToggle/HelpButtonSubEntity.hpp"

namespace Beer::System
{
    class HelpToggle
    {
    private:
        HelpButtonSubEntity* helpEntity = nullptr;
        Function<void> markDirty = nullptr;

        std::unique_ptr<Button> helpButton = nullptr;
        std::unique_ptr<Button> popupButton = nullptr;
        bool popupActive = false;

    public:
        HelpToggle(HelpButtonSubEntity* helpEntity,
            Function<void> markDirty)
            : helpEntity(helpEntity)
            , markDirty(markDirty)
        {
            this->helpEntity->SetPopupEnabled(popupActive);
            helpButton = this->helpEntity->CreateHelpButton();
            popupButton = this->helpEntity->CreatePopupButton();

            Function<void> onHelpClicked = [this]() -> void {
                TogglePopup();
            };

            helpButton->SetOnClick(onHelpClicked);
            popupButton->SetOnClick(onHelpClicked);
            this->markDirty();
        }

    private:
        void TogglePopup()
        {
            popupActive = !popupActive;
            helpEntity->SetPopupEnabled(popupActive);
            markDirty();
        }
    };
} // namespace Beer::System
