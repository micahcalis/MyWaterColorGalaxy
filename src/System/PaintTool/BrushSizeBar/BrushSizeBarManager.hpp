#pragma once

#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/Slider.hpp"
#include "System/Components/UI/UITransform.hpp"
#include <memory>

namespace Beer::System
{
    class BrushSizeBarManager : public IEntityManager
    {
    private:
        std::unique_ptr<Slider> brushSizeSlider = nullptr;
        Function<void, float> setBrushSize = nullptr;

    public:
        BrushSizeBarManager(Function<void, float> setBrushSize)
            : setBrushSize(setBrushSize)
        {
        }

        void InitializeSlider(UITransform* colliderTransform,
            Rendering::Material* spriteMaterial,
            UITransform* buttonTransform)
        {
            brushSizeSlider = std::make_unique<Slider>(colliderTransform,
                spriteMaterial,
                buttonTransform,
                0.5f,
                SliderType::Vertical,
                SliderDirection::Negative);

            brushSizeSlider->OnSliderChanged.Subscribe(setBrushSize);
        }

        bool SliderInitialized() const
        {
            return brushSizeSlider != nullptr;
        }

        void Update() override
        {
        }
    };
} // namespace Beer::System
