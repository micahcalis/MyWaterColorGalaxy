#include "System/PaintTool/GalaxyMap/GalaxyMapZoomer.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "Rendering/Shader/Shader.hpp"
#include "System/Base/Clock/Clock.hpp"

namespace Beer::System
{
    static const float ZOOM_SPEED = 70.0f;
    static const float MIN_ZOOM = 1.0f;
    static const float MAX_ZOOM = 5.0f;

    void GalaxyMapZoomer::Update(bool insideRect)
    {
        MouseInput input = getMouseInput();

        UpdateZoom(input.PixelPos, insideRect ? input.ScrollVector : 0);

        glm::vec2 mouseDelta = input.PixelPos - previousMousePos;
        UpdatePanning(mouseDelta, input.MiddleClickHold && insideRect);

        ClampBounds();

        Rendering::Shader::Globals()->SetGalaxyZoom(Zoom, Panning);
        previousMousePos = input.PixelPos;
    }

    void GalaxyMapZoomer::UpdateZoom(glm::vec2 pixelPos, float scrollVector)
    {
        glm::vec2 screenSize = glm::vec2(Core::Screen::Width(), Core::Screen::Height());
        glm::vec2 normalizedMousePos = pixelPos / screenSize;

        if (scrollVector != 0.0f)
        {
            CalculateZoom(normalizedMousePos, scrollVector);
        }
    }

    void GalaxyMapZoomer::UpdatePanning(glm::vec2 mouseDelta, bool middleClickHold)
    {
        glm::vec2 screenSize = glm::vec2(Core::Screen::Width(), Core::Screen::Height());

        if (middleClickHold)
        {
            CalculatePanning(mouseDelta, screenSize);
        }
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

    void GalaxyMapZoomer::CalculatePanning(glm::vec2 mouseDelta, glm::vec2 screenSize)
    {
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
