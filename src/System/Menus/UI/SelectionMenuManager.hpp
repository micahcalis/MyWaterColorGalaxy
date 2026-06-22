#pragma once

#include "System/Audio/AudioClip.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Menus/UI/MapSlot.hpp"
#include "System/Components/Registry/IEntityManager.hpp"
#include <print>
#include "System/Audio/SoundGlobalSettings.hpp"

namespace Beer::System
{
    static constexpr std::string_view SLOT_MAP_NAME_BASE = "Slot_";

    class SelectionMenuManager : public IEntityManager
    {
    public:
        BeerEvent<void(const std::string&)> OnSlotSelected;

    private:
        std::vector<std::unique_ptr<MapSlot>> mapSlots;
        std::shared_ptr<AudioClip> selectClip = nullptr;

    public:
        SelectionMenuManager()
        {
            AudioSettings audioSettings{};
            audioSettings.Volume = SELECT_CLIP_VOLUME;
            selectClip = std::make_shared<AudioClip>("SoundEffects/UI/Audio_SelectButton",
                audioSettings);
        }

        void InitializeSlots(const std::vector<std::unique_ptr<UISubEntity>>& subEntities,
            const std::vector<std::shared_ptr<Rendering::Material>> materials,
            const uint32_t buttonCount)
        {
            Function<void, const std::string&> managerCallback = [this](const std::string& mapName) -> void {
                HandleSlotClick(mapName);
                selectClip->Play();
            };

            for (uint32_t i = 0; i < buttonCount; i++)
            {
                auto mapSlot = std::make_unique<MapSlot>(subEntities[i]->GetTransform(),
                    materials[i].get(),
                    std::format("{}{}", SLOT_MAP_NAME_BASE, i));

                mapSlot->OnSlotClicked.Subscribe(managerCallback);

                mapSlots.push_back(std::move(mapSlot));
            }
        }

        void Update() override
        {
        }

    private:
        void HandleSlotClick(const std::string& mapName)
        {
            OnSlotSelected.Invoke(mapName);
        }
    };
} // namespace Beer::System
