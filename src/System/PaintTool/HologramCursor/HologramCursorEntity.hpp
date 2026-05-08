#pragma once

#include "HologramCursorManager.hpp"
#include "Rendering/Material/Material.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/UI/UIRenderItem.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/GalaxyMap/GalaxyMapManager.hpp"
#include <memory>

namespace Beer::System
{
    class HologramCursorEntity : public QuadTreeEntity
    {
        std::shared_ptr<Rendering::Material> cursorMaterial = nullptr;
        Function<CursorState> getCursorState = nullptr;
        Function<MouseInput> getMouseInput = nullptr;
        Function<Rendering::Texture2D*, GalaxyBrushType> getBrushTexture = nullptr;

    public:
        HologramCursorEntity(Function<CursorState> getCursorState,
            Function<MouseInput> getMouseInput,
            Function<Rendering::Texture2D*, GalaxyBrushType> getBrushTexture);

        void Update() override
        {
            manager->Update();
            QuadTreeEntity::Update();
        }

        HologramCursorManager* GetCursorManager() const { return static_cast<HologramCursorManager*>(manager.get()); }

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<HologramCursorManager>(&rootTransform,
                cursorMaterial.get(),
                getCursorState,
                getMouseInput,
                getBrushTexture);

            GetCursorManager()->OnDirty.Subscribe([this]() -> void { MarkDirty(); });
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            return {UIRenderItem(&rootTransform, cursorMaterial.get())};
        }
    };
} // namespace Beer::System
