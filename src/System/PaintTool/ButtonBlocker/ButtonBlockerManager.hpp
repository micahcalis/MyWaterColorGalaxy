#pragma once

#include "System/Components/Registry/IEntityManager.hpp"
#include "System/Components/UI/Button.hpp"
#include "System/Components/UI/UITransform.hpp"
namespace Beer::System
{
    class ButtonBlockerManager : public IEntityManager
    {
    private:
        std::unique_ptr<Button> blocker = nullptr;

    public:
        void InitializeBlocker(UITransform* transform,
            Rendering::Material* material)
        {
            blocker = std::make_unique<Button>(transform, material);
        }

        void Update() override
        {
        }
    };
} // namespace Beer::System
