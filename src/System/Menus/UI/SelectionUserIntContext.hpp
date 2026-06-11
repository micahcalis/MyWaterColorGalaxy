#pragma once

#include "Rendering/RenderPasses/DrawUIPass.hpp"
#include "System/Base/Input/ButtonInput.hpp"
#include "System/Menus/UI/SelectionMenuEntity.hpp"
#include "System/Context/IContext.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include <string>

namespace Beer::System
{
    class SelectionUserIntContext : public IContext
    {
    public:
        BeerEvent<void()> OnReturnToTitle;
        BeerEvent<void(const std::string&)> OnSelectMap;

    private:
        Rendering::DrawUIPass* drawUIPass = nullptr;
        SelectionMenuEntity* selectionMenuEntity = nullptr;
        Function<ButtonInput> getReturnKeyInput = nullptr;

    public:
        SelectionUserIntContext(Function<ButtonInput> getReturnKeyInput)
            : getReturnKeyInput(getReturnKeyInput)
        {
        }

        void Load() override;
        void Update() override;
        std::vector<Rendering::IRenderPass*> GetRenderPasses() override;

    private:
        void InitializeSelectionMenu();
    };
} // namespace Beer::System
