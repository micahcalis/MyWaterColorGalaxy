#pragma once

#include "System/Components/UI/Button.hpp"
#include <print>

namespace Beer::System
{
    enum class PigmentType : int
    {
        QuinacridoneRose = 0,
        IndianRed = 1,
        CadmiumYellow = 2,
        HookersGreen = 3,
        CeruleanBlue = 4,
        BurntUmber = 5,
        CadmiumRed = 6,
        BrilliantOrange = 7,
        HansaYellow = 8,
        PhthaloGreen = 9,
        FrenchUltramarine = 10,
        InterferenceLilac = 11
    };

    constexpr glm::vec4 GetPigmentColor(PigmentType pigment)
    {
        switch (pigment)
        {
        case PigmentType::QuinacridoneRose:
            return glm::vec4(0.922f, 0.149f, 0.588f, 1.0f);
        case PigmentType::IndianRed:
            return glm::vec4(0.5f, 0.12f, 0.07f, 1.0f);
        case PigmentType::CadmiumYellow:
            return glm::vec4(1.000f, 0.890f, 0.012f, 1.0f);
        case PigmentType::HookersGreen:
            return glm::vec4(0.02, 0.5f, 0.035f, 1.0f);
        case PigmentType::CeruleanBlue:
            return glm::vec4(0.1f, 0.7f, 0.95f, 1.0f);
        case PigmentType::BurntUmber:
            return glm::vec4(0.3f, 0.07f, 0.000f, 1.0f);
        case PigmentType::CadmiumRed:
            return glm::vec4(1.0f, 0.2f, 0.082f, 1.0f);
        case PigmentType::BrilliantOrange:
            return glm::vec4(0.82f, 0.45f, 0.0f, 1.0f);
        case PigmentType::HansaYellow:
            return glm::vec4(1.0f, 1.0f, 0.04f, 1.0f);
        case PigmentType::PhthaloGreen:
            return glm::vec4(0.1f, 0.75f, 0.5f, 1.0f);
        case PigmentType::FrenchUltramarine:
            return glm::vec4(0.1f, 0.05f, 1.0f, 1.0f);
        case PigmentType::InterferenceLilac:
            return glm::vec4(0.690f, 0.624f, 0.792f, 1.0f);
        default:
            return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }

    class PigmentButton
    {
    private:
        std::unique_ptr<Button> buttonHandle;
        PigmentType pigment;
        Function<void, PigmentType> setPigment;

    public:
        PigmentButton(UITransform* transform,
            Rendering::Material* spriteMaterial,
            PigmentType pigment,
            Function<void, PigmentType> setPigment)
            : pigment(pigment), setPigment(setPigment)
        {
            buttonHandle = std::make_unique<Button>(transform, spriteMaterial);
            buttonHandle->SetOnClick([this]() -> void { ClickedCallback(); });
        }

    private:
        void ClickedCallback()
        {
            if (setPigment == nullptr)
                return;

            setPigment(pigment);
        }
    };
} // namespace Beer::System
