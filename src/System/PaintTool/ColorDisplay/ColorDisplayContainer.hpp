#pragma once

#include "System/PaintTool/ColorDisplay/ColorDisplayHandler.hpp"
#include "System/PaintTool/ColorDisplay/ColorDisplaySubEntity.hpp"

namespace Beer::System
{
    static const std::array<ColorBarLevel, 4> COLOR_DISPLAY_LEVELS = {
        ColorBarLevel::Quaternary,
        ColorBarLevel::Tertiary,
        ColorBarLevel::Secondary,
        ColorBarLevel::Primary};

    class ColorDisplayContainer
    {
    protected:
        std::unique_ptr<ColorDisplayHandler> colorDisplayHandler = std::make_unique<ColorDisplayHandler>();

    public:
        virtual ~ColorDisplayContainer() = default;

        [[nodiscard]] ColorDisplayHandler* GetColorDisplayHandler() const
        {
            if (colorDisplayHandler == nullptr)
                return nullptr;

            return colorDisplayHandler.get();
        }

        void InitializeColorDisplay(ColorDisplaySubEntity* displaySubEntity)
        {
            auto materials = displaySubEntity->GetColorMaterials();

            for (int i = 0; i < 4; i++)
            {
                colorDisplayHandler->AddLevel(COLOR_DISPLAY_LEVELS[i],
                    materials[i]);
            }
        }
    };
} // namespace Beer::System
