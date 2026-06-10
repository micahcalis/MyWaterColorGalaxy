#include "System/PaintTool/GalaxyMap//GalaxyMapManager.hpp"
#include "GalaxyComponent.hpp"
#include "GalaxyMapBuffer.hpp"
#include "GalaxyMapZoomer.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapCursor.hpp"
#include <memory>
#include <print>

namespace Beer::System
{
    GalaxyMapManager::GalaxyMapManager(GalaxyMapBuffer* galaxyMapBuffer,
        UITransform* mapTransform,
        Function<MouseInput> getMouseInput,
        UITransform* playerIndicatorTransform)
        : galaxyBuffer(galaxyMapBuffer)
        , mapTransform(mapTransform)
        , getMouseInput(getMouseInput)
        , playerIndicatorTransform(playerIndicatorTransform)
    {
    }

    void GalaxyMapManager::InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor,
        UITransform* sunTranform)
    {
        zoomer = std::make_unique<GalaxyMapZoomer>(getMouseInput);

        cursor = std::make_unique<GalaxyMapCursor>(galaxyBuffer,
            mapTransform,
            sunTranform,
            getColor);
    }

    void GalaxyMapManager::Update()
    {
        if (cursor != nullptr)
        {
            MouseInput mouseInput = getMouseInput();
            if (QuadCollider::Hit(mapTransform, mouseInput.PixelPos))
            {
                zoomer->Update();
                cursor->Update(mouseInput, zoomer->Zoom, zoomer->Panning);
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
        OnNewBrush.Invoke(cursor->Brush);
    }

    void GalaxyMapManager::ReloadFromSerialized(const SerializableGalaxyMap& serializedData)
    {
        galaxyBuffer->ApplySerializableGalaxy(serializedData.Galaxy);

        GalaxyComponentData data{};

        for (const auto& serializedComp : serializedData.Galaxy.Components)
        {
            data.Brush = static_cast<GalaxyBrushType>(serializedComp.TypeIndex);
            data.Colors = serializedComp.Colors;
            data.Position = serializedComp.Position;
            data.Scale = serializedComp.Scale;
            data.Tilt = serializedComp.Tilt;
            cursor->Place(data);
        }

        cursor->Size = serializedData.ToolHistory.BrushSize;
        cursor->Brush = static_cast<GalaxyBrushType>(serializedData.ToolHistory.SelectedType);

        zoomer->Zoom = serializedData.ToolHistory.ZoomScale;
        zoomer->Panning = serializedData.ToolHistory.ZoomPanning;

        playerIndicatorTransform->Position = glm::vec2(serializedData.ExplorerHistory.PlayerPosition.x, serializedData.ExplorerHistory.PlayerPosition.z) * mapTransform->Scale;
    }
} // namespace Beer::System
