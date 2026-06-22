#include "System/PaintTool/GalaxyMap/GalaxyMapCursor.hpp"
#include "Core/Application/Renderer/Screen.hpp"
#include "GalaxyBrushType.hpp"
#include "GalaxyComponent.hpp"
#include "GalaxyMapBuffer.hpp"
#include "GalaxyMapCursor.hpp"
#include "System/Audio/AudioClip.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/PaintTool/ColorBar/ColorBarLevel.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySeed.hpp"
#include "System/PaintTool/GalaxyMap/GalaxySpriteFactory.hpp"
#include <memory>
#include "System/Audio/SoundGlobalSettings.hpp"

namespace Beer::System
{
    static const float CURSOR_MIN = 0.02f;
    static const float CURSOR_MAX = 0.15f;
    static const float TILT_SCALE = glm::radians(50.0f);
    static const float ZOOM_SENSITIVITY = 6.0f;

    GalaxyMapCursor::GalaxyMapCursor(GalaxyMapBuffer* galaxyMapBuffer,
        UITransform* mapTransform,
        UITransform* sunTransform,
        Function<glm::vec4, ColorBarLevel> getColor,
        Function<void, glm::vec2, float> setZoom,
        Function<void, glm::vec2> setPanning)
        : galaxyMapBuffer(galaxyMapBuffer)
        , mapTransform(mapTransform)
        , sunTransform(sunTransform)
        , getColor(getColor)
        , setZoom(setZoom)
        , setPanning(setPanning)
    {
        factory = std::make_unique<GalaxySpriteFactory>();
        SetSize(0.5f);

        AudioSettings audioSettings{};
        audioSettings.Volume = PLACE_MAP_CLIP_VOLUME;
        placeAudioClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_PlaceOnMap",
            audioSettings);

        audioSettings.Volume = ERASE_CLIP_VOLUME;
        eraseAudioClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_Erase",
            audioSettings);
    }

    void GalaxyMapCursor::Update(MouseInput input,
        float zoomScale,
        glm::vec2 panning,
        bool insideRect)
    {
        glm::vec2 zoomedMousePos = ApplyZoom(input.PixelPos, zoomScale, panning);

        PixelRect cursorRect = GetCursorRect(zoomedMousePos);
        GalaxyComponentHitInfo hitInfo = galaxyMapBuffer->CollisionCheck(cursorRect);

        bool overSun = cursorRect.Intersects(sunTransform->Rect);

        if (Brush == GalaxyBrushType::Hand || Brush == GalaxyBrushType::MagnifyingGlass)
        {
            CanUseCursor = true;
        } else if (overSun)
        {
            CanUseCursor = false;
            return;
        }

        bool hit = hitInfo.Hit;

        if (IsGalaxyComponent(Brush))
        {
            if (!hit && input.leftClickStart && insideRect)
            {
                glm::vec2 mapSpacePosition = ToMapSpace(zoomedMousePos);
                Place(GetNewData(mapSpacePosition));
                placeAudioClip->Play();
            }
            CanUseCursor = !hit;
        } else if (Brush == GalaxyBrushType::Eraser)
        {
            if (hit && input.LeftClickHold && insideRect)
            {
                Erase(hitInfo.Index);
                eraseAudioClip->Play();
            }
            CanUseCursor = hit;
        } else if (Brush == GalaxyBrushType::MagnifyingGlass)
        {
            if (insideRect && input.leftClickStart)
            {
                SetZoomAnchor(input.PixelPos);
                zooming = true;
            } else if (input.LeftClickHold && zooming)
            {
                Zoom(input.PixelPos);
            } else if (input.LeftClickEnd)
            {
                zooming = false;
            }
        } else if (Brush == GalaxyBrushType::Hand)
        {
            if (insideRect && input.leftClickStart)
            {
                isPanning = true;
                lastMousePos = input.PixelPos;
            } else if (input.LeftClickHold && isPanning)
            {
                Pan(input.PixelPos);
            } else if (input.LeftClickEnd)
            {
                isPanning = false;
            }
        }
    }

    uint32_t GalaxyMapCursor::Place(const GalaxyComponentData& data, bool fromHistory)
    {
        glm::vec2 screenSpacePosition = mapTransform->Scale * data.Position;
        glm::vec2 screenSpaceScale = mapTransform->Scale * data.Scale;

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
    }

    float GalaxyMapCursor::GetNormalizedSize() const
    {
        return (Size - CURSOR_MIN) / (CURSOR_MAX - CURSOR_MIN);
    }

    glm::vec2 GalaxyMapCursor::ApplyZoom(glm::vec2 rawMouse,
        float zoomScale,
        glm::vec2 panning) const
    {
        glm::vec2 pixelPanning = panning * glm::vec2(Core::Screen::Width(), Core::Screen::Height());
        return (rawMouse - pixelPanning) / zoomScale;
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

        data.Tilt = glm::vec2(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                        static_cast<float>(rand()) / static_cast<float>(RAND_MAX))
            - 0.5f;

        data.Tilt *= TILT_SCALE;

        return data;
    }

    void GalaxyMapCursor::SetZoomAnchor(glm::vec2 initialPos)
    {
        zoomAnchor = initialPos;
        lastMousePos = initialPos;
    }

    void GalaxyMapCursor::Zoom(glm::vec2 pixelPos)
    {
        float offset = lastMousePos.x - pixelPos.x;
        float normalizedOffset = offset / Core::Screen::RectUnitLength();

        setZoom(zoomAnchor, normalizedOffset * ZOOM_SENSITIVITY);
        lastMousePos = pixelPos;
    }

    void GalaxyMapCursor::Pan(glm::vec2 pixelPos)
    {
        glm::vec2 delta = pixelPos - lastMousePos;
        setPanning(delta);
        lastMousePos = pixelPos;
    }
} // namespace Beer::System
