#pragma once

#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include <memory>

namespace Beer::System
{
    static const glm::vec4 ENABLED_TINT = glm::vec4(0.6f, 0.6f, 0.6f, 1);

    class GalaxyBrushController
    {
    public:
        BeerEvent<void(GalaxyBrushType)> OnBrushClicked;

    private:
        std::unique_ptr<Button> button = nullptr;
        Rendering::Material* material = nullptr;
        GalaxyBrushType brushType = GalaxyBrushType::Planet;

    public:
        GalaxyBrushController(UITransform* transform,
            Rendering::Material* material,
            GalaxyBrushType brushType)
            : material(material), brushType(brushType)
        {
            button = std::make_unique<Button>(transform, material);
            button->SetOnClick([this]() -> void { ButtonClicked(); });
        }

        void SetActive(bool active)
        {
            material->SetColor("_TintColor", active ? ENABLED_TINT : glm::vec4(1));
        }

    private:
        void ButtonClicked()
        {
            // TODO: Add Guard clauses prob
            OnBrushClicked.Invoke(brushType);
            std::println("Brush Button clicked: {}", magic_enum::enum_name(brushType));
        }
    };
} // namespace Beer::System
