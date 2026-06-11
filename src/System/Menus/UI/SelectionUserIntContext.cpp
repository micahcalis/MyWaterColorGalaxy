#include "System/Menus/UI/SelectionUserIntContext.hpp"
#include "MapSlot.hpp"
#include "SelectionMenuEntity.hpp"
#include "System/Base/Input/ButtonInput.hpp"

namespace Beer::System
{
    void SelectionUserIntContext::Load()
    {
        drawUIPass = Rendering::IRenderPass::FetchFromRegister<Rendering::DrawUIPass>(
            std::string(Rendering::UI_PASS));

        InitializeSelectionMenu();
    }

    void SelectionUserIntContext::Update()
    {
        if (selectionMenuEntity != nullptr)
        {
            selectionMenuEntity->Update();
        }

        ButtonInput returnInput = getReturnKeyInput();

        if (returnInput.ButtonStart)
        {
            OnReturnToTitle.Invoke();
        }
    }

    std::vector<Rendering::IRenderPass*> SelectionUserIntContext::GetRenderPasses()
    {
        return {drawUIPass};
    }

    void SelectionUserIntContext::InitializeSelectionMenu()
    {
        selectionMenuEntity = registry.CreateEntity<SelectionMenuEntity>();
        selectionMenuEntity->InitializeSlots();

        Function<void, const std::string&> onSelectMap = [this](const std::string& mapName) -> void {
            OnSelectMap.Invoke(mapName);
        };

        selectionMenuEntity->GetSelectionMenuManager()->OnSlotSelected.Subscribe(onSelectMap);
    }
} // namespace Beer::System
