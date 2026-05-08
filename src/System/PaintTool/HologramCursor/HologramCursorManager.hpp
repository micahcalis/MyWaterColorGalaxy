#pragma once

#include "Rendering/Material/Material.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include "System/Delegates/Delegate.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyBrushType.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapManager.hpp"

namespace Beer::System
{
    class HologramCursorManager : public IEntityManager
    {
    public:
        BeerEvent<void()> OnDirty;

    private:
        UITransform* cursorTransform = nullptr;
        Rendering::Material* cursorMaterial = nullptr;

        Function<CursorState> getCursorState = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        Function<Rendering::Texture2D*, GalaxyBrushType> getBrushTexture = nullptr;
        CursorState prevState{};

    public:
        HologramCursorManager(UITransform* cursorTransform,
            Rendering::Material* cursorMaterial,
            Function<CursorState> getCursorState,
            Function<MouseInput> getMouseInput,
            Function<Rendering::Texture2D*, GalaxyBrushType> getBrushTexture)
            : cursorTransform(cursorTransform), cursorMaterial(cursorMaterial), getCursorState(getCursorState), getMouseInput(getMouseInput), getBrushTexture(getBrushTexture)
        {
            cursorMaterial->SetTexture("_SpriteTex", getBrushTexture(prevState.Brush));
        }

        void Update() override
        {
            CursorState currentState = getCursorState();

            if (prevState.IsActive != currentState.IsActive)
            {
                SetMaterial(currentState.IsActive, currentState.Brush);
                cursorTransform->SetEnabled(currentState.IsActive);
                OnDirty.Invoke();
            }

            if (currentState.IsActive)
            {
                SetTransform(getMouseInput().PixelPos, currentState.BrushCanvasSize);
                OnDirty.Invoke();
            }

            if (prevState.CanUseCursor != currentState.CanUseCursor
                || prevState.Brush != currentState.Brush)
            {
                SetMaterial(currentState.CanUseCursor, currentState.Brush);
            }

            prevState = currentState;
        }

        void SetTransform(glm::vec2 mousePos, float brushSize);
        void SetMaterial(bool canUseCursor, GalaxyBrushType brush);
    };
} // namespace Beer::System
