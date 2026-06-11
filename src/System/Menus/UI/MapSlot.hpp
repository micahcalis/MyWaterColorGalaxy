#pragma once

#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"

namespace Beer::System
{
    class MapSlot
    {
    public:
        BeerEvent<void(const std::string&)> OnSlotClicked;

    private:
        std::unique_ptr<Button> buttonHandle = nullptr;
        std::string mapName = "";

    public:
        MapSlot(UITransform* transform,
            Rendering::Material* material,
            const std::string& name)
            : mapName(name)
        {
            buttonHandle = std::make_unique<Button>(transform, material);
            buttonHandle->SetOnClick([this]() -> void { OnSlotClicked.Invoke(mapName); });
        }
    };
} // namespace Beer::System
