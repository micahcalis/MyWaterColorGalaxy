#pragma once

#include "System/Components/UI/UITransform.hpp"
#include "vulkan/vulkan.hpp"
#include <cstdint>

namespace Beer::Core
{
    constexpr static float RECT_UNIT_SCALAR = 0.25f;

    class Screen
    {
        friend class Renderer;

    private:
        uint32_t width{};
        uint32_t height{};
        VkFormat colorFormat{};
        VkFormat depthFormat{};
        float aspect;

        System::UITransform screenTransform{};
        float rectUnitLength;

    public:
        Screen() = default;

        Screen(uint32_t width,
            uint32_t height,
            VkFormat colorFormat,
            VkFormat depthFormat)
            : width(width), height(height), colorFormat(colorFormat), depthFormat(depthFormat)
        {
            aspect = static_cast<float>(width) / static_cast<float>(height);
            rectUnitLength = static_cast<float>(width) * RECT_UNIT_SCALAR;

            screenTransform.Pivot = System::AnchorMode::BottomLeft;
            screenTransform.Scale = glm::vec2(aspect / RECT_UNIT_SCALAR, 1.0f / RECT_UNIT_SCALAR);
            screenTransform.Rect = System::PixelRect(glm::vec2(width, height),
                glm::vec2(width, 0),
                glm::vec2(0, height),
                glm::vec2(0, 0));
        }

    private:
        static Screen instance;

        static void SetScreen(const Screen screen)
        {
            instance = screen;
        }

    public:
        static uint32_t Width() { return instance.width; }
        static uint32_t Height() { return instance.height; }
        static VkFormat ColorFormat() { return instance.colorFormat; }
        static VkFormat DepthFormat() { return instance.depthFormat; }
        static float Aspect() { return instance.aspect; }
        static float RectUnitLength() { return instance.rectUnitLength; }
        [[nodiscard]] static System::UITransform* ScreenTransform() { return &instance.screenTransform; }
    };

    inline Screen Screen::instance;
} // namespace Beer::Core
