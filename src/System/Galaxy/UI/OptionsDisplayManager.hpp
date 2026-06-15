#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    class OptionsDisplayManager : public IEntityManager
    {
    private:
        std::unique_ptr<Button> returnButton = nullptr;
        std::unique_ptr<Button> photoButton = nullptr;

    public:
        void Update() override
        {
        }

        void InitializeButtons(UITransform* returnTransform,
            Rendering::Material* returnMaterial,
            UITransform* photoTransform,
            Rendering::Material* photoMaterial,
            Function<void> invokeReturn,
            Function<void> invokePhotoModeToggle)
        {
            returnButton = std::make_unique<Button>(returnTransform,
                returnMaterial);

            returnButton->SetOnClick(invokeReturn);

            photoButton = std::make_unique<Button>(photoTransform,
                photoMaterial);

            photoButton->SetOnClick(invokePhotoModeToggle);
        }
    };

} // namespace Beer::System
