#include "System/PaintTool/GalaxyMap//GalaxyMapManager.hpp"
#include "GalaxyComponent.hpp"
#include "GalaxyMapBuffer.hpp"
#include "GalaxyMapZoomer.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapCursor.hpp"
#include "System/Serialization/SerializableGalaxy.hpp"
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
        SerializableExplorer dummyExplorer{};
        SetPlayerTransformPosition(dummyExplorer.PlayerPosition);
    }

    void GalaxyMapManager::InitializeCursor(Function<glm::vec4, ColorBarLevel> getColor,
        UITransform* sunTranform)
    {
        zoomer = std::make_unique<GalaxyMapZoomer>(getMouseInput);

        Function<void, glm::vec2, float> setZoom = [this](glm::vec2 pixelPos, float offset) -> void {
            zoomer->UpdateZoom(pixelPos, -offset);
        };

        Function<void, glm::vec2> setPanning = [this](glm::vec2 mouseDelta) -> void {
            zoomer->UpdatePanning(mouseDelta, true);
        };

        cursor = std::make_unique<GalaxyMapCursor>(galaxyBuffer,
            mapTransform,
            sunTranform,
            getColor,
            setZoom,
            setPanning);
    }

    void GalaxyMapManager::Update()
    {
        if (cursor != nullptr)
        {
            MouseInput mouseInput = getMouseInput();

            isActive = QuadCollider::Hit(mapTransform, mouseInput.PixelPos);

            cursor->Update(mouseInput,
                zoomer->Zoom,
                zoomer->Panning,
                isActive);

            zoomer->Update(isActive);

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

        SetPlayerTransformPosition(serializedData.ExplorerHistory.PlayerPosition);
    }

    void GalaxyMapManager::SetPlayerTransformPosition(glm::vec3 serializedPosition)
    {
        playerIndicatorTransform->Position = glm::vec2(serializedPosition.x, serializedPosition.z) * mapTransform->Scale;
    }
} // namespace Beer::System
