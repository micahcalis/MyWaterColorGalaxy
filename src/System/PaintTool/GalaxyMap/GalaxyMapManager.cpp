#include "System/PaintTool/GalaxyMap//GalaxyMapManager.hpp"
#include "GalaxyMapBuffer.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapCursor.hpp"

namespace Beer::System
{
    GalaxyMapManager::GalaxyMapManager(GalaxyMapBuffer* galaxyMapBuffer,
        UITransform* mapTransform,
        Function<MouseInput> getMouseInput)
        : galaxyBuffer(galaxyMapBuffer), mapTransform(mapTransform), getMouseInput(getMouseInput)
    {
    }

    void GalaxyMapManager::InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor)
    {
        cursor = std::make_unique<GalaxyMapCursor>(galaxyBuffer,
            mapTransform,
            getColor);
    }

    void GalaxyMapManager::Update()
    {
        if (cursor != nullptr)
        {
            MouseInput mouseInput = getMouseInput();

            if (QuadCollider::Hit(mapTransform, mouseInput.PixelPos))
            {
                cursor->Update(mouseInput);
                isActive = true;
            } else
            {
                isActive = false;
            }
            return;
        }

        isActive = false;
    }

    void GalaxyMapManager::SetBrushType(GalaxyBrushType type)
    {
        if (cursor == nullptr)
            return;

        cursor->Brush = type;
    }
} // namespace Beer::System
