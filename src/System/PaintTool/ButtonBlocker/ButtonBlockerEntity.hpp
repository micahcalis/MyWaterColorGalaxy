#pragma once

#include "ButtonBlockerManager.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Context/ContextType.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"

namespace Beer::System
{
    class ButtonBlockerEntity : public QuadTreeEntity
    {
    private:
        std::shared_ptr<Rendering::Material> emptyMaterial = nullptr;

    public:
        ButtonBlockerEntity(ContextType context)
            : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
        {
            rootTransform.Scale = glm::vec2(1000);
            rootTransform.Depth = 1000;
        }

        void InitializeBlocker()
        {
            emptyMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
            GetButtonBlockerManager()->InitializeBlocker(&rootTransform, emptyMaterial.get());
        }

        [[nodiscard]] ButtonBlockerManager* GetButtonBlockerManager() const { return static_cast<ButtonBlockerManager*>(manager.get()); };

    protected:
        void InitializeManager() override
        {
            manager = std::make_unique<ButtonBlockerManager>();
        }

        std::vector<UIRenderItem> GetRenderItems() override
        {
            return {};
        }
    };
} // namespace Beer::System
