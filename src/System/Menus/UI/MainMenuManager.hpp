#pragma once

#include "Rendering/Material/Material.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"

namespace Beer::System
{
    class MainMenuManager : public IEntityManager
    {
    public:
        BeerEvent<void()> OnStart;
        BeerEvent<void()> OnExit;

    private:
        std::unique_ptr<Button> startButton = nullptr;
        std::unique_ptr<Button> exitButton = nullptr;

    public:
        void InitializeButtons(UITransform* startTransform,
            Rendering::Material* startMaterial,
            UITransform* exitTransform,
            Rendering::Material* exitMaterial)
        {
            startButton = std::make_unique<Button>(startTransform,
                startMaterial);

            startButton->SetOnClick([this]() -> void { OnStart.Invoke(); });

            exitButton = std::make_unique<Button>(exitTransform,
                exitMaterial);

            exitButton->SetOnClick([this]() -> void { OnExit.Invoke(); });
        }

        void Update() override
        {
        }
    };
} // namespace Beer::System
