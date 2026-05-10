#include "System/PaintTool/GalaxyMap/GalaxyMapZoomer.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Base/Clock/Clock.hpp"

namespace Beer::System
{
    static const float ZOOM_SPEED = 70.0f;
    static const float MIN_ZOOM = 1.0f;
    static const float MAX_ZOOM = 5.0f;

    void GalaxyMapZoomer::Update()
    {
        MouseInput input = getMouseInput();
        glm::vec2 screenSize = glm::vec2(Core::Screen::Width(), Core::Screen::Height());
        glm::vec2 normalizedMousePos = input.PixelPos / screenSize;

        if (input.ScrollVector != 0.0f)
        {
            CalculateZoom(normalizedMousePos, input.ScrollVector);
        }

        if (input.MiddleClickHold)
        {
            CalculatePanning(input.PixelPos, screenSize);
        }

        ClampBounds();

        Rendering::Shader::Globals()->SetGalaxyZoom(Zoom, Panning);
        previousMousePos = input.PixelPos;
    }

    void GalaxyMapZoomer::CalculateZoom(glm::vec2 normalizedMousePos, float scrollVector)
    {
        float oldZoom = Zoom;
        Zoom += ZOOM_SPEED * scrollVector * (float)Clock::DeltaTime();
        Zoom = std::clamp(Zoom, MIN_ZOOM, MAX_ZOOM);

        if (Zoom != oldZoom)
        {
            Panning = normalizedMousePos - (normalizedMousePos - Panning) * (Zoom / oldZoom);
        }
    }

    void GalaxyMapZoomer::CalculatePanning(glm::vec2 mousePos, glm::vec2 screenSize)
    {
        glm::vec2 mouseDelta = mousePos - previousMousePos;
        glm::vec2 normMouseDelta = mouseDelta / screenSize;
        Panning += normMouseDelta;
    }

    void GalaxyMapZoomer::ClampBounds()
    {
        float maxPan = 0.0f;
        float minPan = 1.0f - Zoom;

        Panning.x = std::clamp(Panning.x, minPan, maxPan);
        Panning.y = std::clamp(Panning.y, minPan, maxPan);
    }
} // namespace Beer::System
