#pragma once

#include "ExitPhotoModeManager.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include <memory>
namespace Beer::System
{
    class ExitPhotoModeEntity : public QuadTreeEntity
    {
    private:
        std::unique_ptr<UISubEntity> exitPhotoModeEntity = nullptr;
        std::shared_ptr<Rendering::Texture2D> exitPhotoModeSprite = nullptr;
        std::shared_ptr<Rendering::Material> exitPhotModeMaterial = nullptr;

    public:
        ExitPhotoModeEntity();

        void InitializeButton(Function<void> exitPhotoMode)
        {
            GetExitPhotoModeManager()->InitializeButton(exitPhotoModeEntity->GetTransform(),
                exitPhotModeMaterial.get(),
                exitPhotoMode);
        }

        ExitPhotoModeManager* GetExitPhotoModeManager() const
        {
            return static_cast<ExitPhotoModeManager*>(manager.get());
        }

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<ExitPhotoModeManager>();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            std::vector<UIRenderItem> renderItems;
            renderItems.push_back(UIRenderItem(exitPhotoModeEntity.get(), exitPhotModeMaterial.get()));
            return renderItems;
        }
    };
} // namespace Beer::System
