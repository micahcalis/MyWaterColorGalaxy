#pragma once

#include "PigmentButton.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <vector>

namespace Beer::System
{
    class ColorMixerManager : public IEntityManager
    {
    private:
        std::vector<std::unique_ptr<PigmentButton>> pigmentButtons;
        PigmentType currentPigment = PigmentType::QuinacridoneRose;

    public:
        void Update() override;

        void AddPigmentButton(UITransform* transform,
            Rendering::Material* spriteMaterial,
            PigmentType pigment);

        void SetCurrentPigment(PigmentType pigment);
        PigmentType GetCurrentPigment() const { return currentPigment; }
    };
} // namespace Beer::System
