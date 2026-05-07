#include "System/PaintTool/GalaxyMap/GalaxyMapCursor.hpp"
#include "GalaxyComponent.hpp"
#include "GalaxyMapBuffer.hpp"
#include "GalaxyMapCursor.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySeed.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySpriteFactory.hpp"
#include <memory>
#include <print>

namespace Beer::System
{
    static const float CURSOR_MIN = 0.02f;
    static const float CURSOR_MAX = 0.15f;

    GalaxyMapCursor::GalaxyMapCursor(GalaxyMapBuffer* galaxyMapBuffer,
        UITransform* mapTransform,
        Function<glm::vec4, ColorBarLevel> getColor)
        : galaxyMapBuffer(galaxyMapBuffer), mapTransform(mapTransform), getColor(getColor)
    {
        factory = std::make_unique<GalaxySpriteFactory>();
        SetSize(0.5f);
    }

    void GalaxyMapCursor::Update(MouseInput input)
    {
        PixelRect cursorRect = GetCursorRect(input.PixelPos);
        GalaxyComponentHitInfo hitInfo = galaxyMapBuffer->CollisionCheck(cursorRect);

        hit = hitInfo.Hit;

        if (Brush != GalaxyBrushType::Eraser)
        {
            if (!hit && input.leftClickStart)
            {
                glm::vec2 mapSpacePosition = ToMapSpace(input.PixelPos);
                Place(GetNewData(mapSpacePosition));
            }
        } else
        {
            if (hit && input.leftClickStart)
            {
                Erase(hitInfo.Index);
            }
        }
    }

    uint32_t GalaxyMapCursor::Place(const GalaxyComponentData& data, bool fromHistory)
    {
        glm::vec2 screenSpacePosition = mapTransform->Scale * data.Position;
        glm::vec2 screenSpaceScale = mapTransform->Scale * Size;

        UITransform componentTransform{};
        componentTransform.Position = screenSpacePosition;
        componentTransform.Scale = screenSpaceScale;
        componentTransform.Anchor = AnchorMode::BottomLeft;
        componentTransform.Pivot = AnchorMode::Center;

        std::unique_ptr<UISubEntity> componentEntity = std::make_unique<UISubEntity>(componentTransform);
        mapTransform->BindChild(componentEntity->GetTransform());

        std::shared_ptr<Rendering::Material> componentMaterial = factory->GetMaterial(data);

        uint32_t index = galaxyMapBuffer->AddComponent(GalaxyComponent(std::move(componentEntity),
            componentMaterial,
            data));

        OnComponentPlaced.Invoke(index, data, fromHistory);
        return index;
    }

    void GalaxyMapCursor::Erase(uint32_t index, bool fromHistory)
    {
        if (!galaxyMapBuffer->HasComponent(index))
            return;

        GalaxyComponentData data = galaxyMapBuffer->GetComponentData(index);
        galaxyMapBuffer->RemoveComponent(index);
        OnComponentErased.Invoke(index, data, fromHistory);
    }

    void GalaxyMapCursor::SetSize(float normalizedVal)
    {
        Size = glm::mix(CURSOR_MIN, CURSOR_MAX, normalizedVal);
        std::println("Size: {}", Size);
    }

    PixelRect GalaxyMapCursor::GetCursorRect(glm::vec2 mousePos) const
    {
        glm::vec2 pixelHalfSize = ScaleToPixelSize(glm::vec2(Size, Size)) * 0.5f;

        PixelRect mapRect = mapTransform->Rect;
        glm::vec2 xDir = glm::normalize(mapRect.BotRight - mapRect.BotLeft);
        glm::vec2 yDir = glm::normalize(mapRect.TopLeft - mapRect.BotLeft);

        glm::vec2 rightOffset = xDir * pixelHalfSize.x;
        glm::vec2 upOffset = yDir * pixelHalfSize.y;

        return PixelRect(mousePos + rightOffset + upOffset,
            mousePos + rightOffset - upOffset,
            mousePos - rightOffset + upOffset,
            mousePos - rightOffset - upOffset);
    }

    glm::vec2 GalaxyMapCursor::ToMapSpace(glm::vec2 pixelVec) const
    {
        PixelRect mapRect = mapTransform->Rect;

        glm::vec2 localPos = pixelVec - mapRect.BotLeft;

        glm::vec2 xAxis = mapRect.BotRight - mapRect.BotLeft;
        glm::vec2 yAxis = mapRect.TopLeft - mapRect.BotLeft;

        float u = glm::dot(localPos, xAxis) / glm::dot(xAxis, xAxis);
        float v = glm::dot(localPos, yAxis) / glm::dot(yAxis, yAxis);

        return glm::vec2(u, v);
    }

    glm::vec2 GalaxyMapCursor::ToPixelSpace(glm::vec2 mapVec) const
    {
        PixelRect mapRect = mapTransform->Rect;

        glm::vec2 xAxis = mapRect.BotRight - mapRect.BotLeft;
        glm::vec2 yAxis = mapRect.TopLeft - mapRect.BotLeft;

        return mapRect.BotLeft + (mapVec.x * xAxis) + (mapVec.y * yAxis);
    }

    glm::vec2 GalaxyMapCursor::ScaleToPixelSize(glm::vec2 mapVec) const
    {
        PixelRect mapRect = mapTransform->Rect;

        float widthPixels = glm::length(mapRect.BotRight - mapRect.BotLeft);
        float heightPixels = glm::length(mapRect.TopLeft - mapRect.BotLeft);

        return glm::vec2(mapVec.x * widthPixels, mapVec.y * heightPixels);
    }

    GalaxyComponentData GalaxyMapCursor::GetNewData(glm::vec2 mapSpacePosition) const
    {
        GalaxyComponentData data{};
        data.Brush = Brush;

        data.Colors = {getColor(ColorBarLevel::Primary),
            getColor(ColorBarLevel::Secondary),
            getColor(ColorBarLevel::Tertiary),
            getColor(ColorBarLevel::Quaternary)};

        data.Position = mapSpacePosition;
        data.Scale = Size;
        return data;
    }
} // namespace Beer::System
