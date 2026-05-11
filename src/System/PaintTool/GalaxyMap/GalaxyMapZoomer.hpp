#pragma once

#include "System/Base/Input/MouseInput.hpp"
#include "System/Delegates/Delegate.hpp"

namespace Beer::System
{
    class GalaxyMapZoomer
    {
    public:
        float Zoom = 1;
        glm::vec2 Panning = glm::vec2(0);

    private:
        Function<MouseInput> getMouseInput = nullptr;
        glm::vec2 previousMousePos;

    public:
        GalaxyMapZoomer(Function<MouseInput> getMouseInput)
            : getMouseInput(getMouseInput)
        {
            previousMousePos = getMouseInput().PixelPos;
        }

        void Update();

    private:
        void CalculateZoom(glm::vec2 normalizedMousePos, float scrollVector);
        void CalculatePanning(glm::vec2 mousePos, glm::vec2 screenSize);
        void ClampBounds();
    };
} // namespace Beer::System
