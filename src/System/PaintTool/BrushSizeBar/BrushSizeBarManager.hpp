#pragma once

#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/Slider.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/HelpToggle/HelpButtonSubEntity.hpp"
#include "System/PaintTool/HelpToggle/HelpContainer.hpp"
#include "System/PaintTool/HelpToggle/HelpToggle.hpp"
#include <memory>

namespace Beer::System
{
    class BrushSizeBarManager : public IEntityManager
        , public HelpContainer
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
                SliderType::Horizontal,
                SliderDirection::Positive);

            brushSizeSlider->OnSliderChanged.Subscribe(setBrushSize);
        }

        Slider* GetSlider() const { return brushSizeSlider.get(); }

        bool SliderInitialized() const
        {
            return brushSizeSlider != nullptr;
        }

        void Update() override
        {
        }
    };
} // namespace Beer::System
