#pragma once

#include "PigmentButton.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/Delegate.hpp"
#include <vector>

namespace Beer::System
{
    class ColorMixerManager : public IEntityManager
    {
    private:
        std::vector<std::unique_ptr<PigmentButton>> pigmentButtons;
        PigmentType currentPigment = PigmentType::QuinacridoneRose;
        std::unique_ptr<Button> clearButton = nullptr;
        Function<void> clearColorMixer = nullptr;

    public:
        void Update() override;

        void AddPigmentButton(UITransform* transform,
            Rendering::Material* spriteMaterial,
            PigmentType pigment);

        void SetCurrentPigment(PigmentType pigment);
        PigmentType GetCurrentPigment() const { return currentPigment; }

        void SetClearButton(Function<void> clearColorMixer,
            UITransform* clearTransform,
            Rendering::Material* clearMaterial);

    private:
        void ClearColorMixer();
    };
} // namespace Beer::System
