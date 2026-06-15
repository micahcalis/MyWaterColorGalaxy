#pragma once

#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class ExitPhotoModeManager : public IEntityManager
    {
    private:
        std::unique_ptr<Button> exitPhotoModeButton = nullptr;

    public:
        void Update() override
        {
        }

        void InitializeButton(UITransform* exitPhotoModeTransform,
            Rendering::Material* exitPhotoModeMaterial,
            Function<void> exitPhotoMode)
        {
            exitPhotoModeButton = std::make_unique<Button>(exitPhotoModeTransform,
                exitPhotoModeMaterial);

            exitPhotoModeButton->SetOnClick(exitPhotoMode);
        }
    };
} // namespace Beer::System
