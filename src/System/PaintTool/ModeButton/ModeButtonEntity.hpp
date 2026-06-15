#pragma once

#include "System/Base/Input/ButtonInput.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/PaintTool/ModeButton/ModeButtonManager.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"

namespace Beer::System
{
    class ModeButtonEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> modeButtonMaterial = nullptr;
        std::shared_ptr<Rendering::Texture2D> colorModeTexture = nullptr;
        std::shared_ptr<Rendering::Texture2D> brushModeTexture = nullptr;

        Function<void, bool> setColorModeActive = nullptr;
        Function<void, bool> setBrushModeActive = nullptr;
        Function<ButtonInput> getTabKeyInput = nullptr;

    public:
        ModeButtonEntity(Function<void, bool> setColorModeActive,
            Function<void, bool> setBrushModeActive,
            Function<ButtonInput> getTabKeyInput);

        void InitializeModeButton(BrushMenuMode menuMode = BrushMenuMode::ToolMenu);

        void Update() override
        {
            QuadTreeEntity::Update();
            manager->Update();
        }

        void InitializeManager() override
        {
            manager = std::make_unique<ModeButtonManager>(getTabKeyInput);
        }

        [[nodiscard]] ModeButtonManager* GetModeButtonManager() const { return static_cast<ModeButtonManager*>(manager.get()); }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            return {UIRenderItem(&rootTransform, modeButtonMaterial.get())};
        }
    };
} // namespace Beer::System
