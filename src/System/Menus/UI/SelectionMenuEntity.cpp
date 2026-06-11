#include "System/Menus/UI/SelectionMenuEntity.hpp"
#include "Rendering/RenderPasses/Painting/WaterColorSimBuffers.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "SelectionMenuEntity.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"

namespace Beer::System
{
    static const glm::vec2 HEADER_SIZE = glm::vec2(1.0f, 0.25f);
    static const glm::vec2 HEADER_OFFSET = glm::vec2(0.0f, 0.1f);

    static const glm::vec2 PANEL_SIZE = glm::vec2(1.425f, 0.9f);

    static const uint32_t SLOT_COLUMNS = 8;
    static const uint32_t SLOT_ROWS = 5;
    static const uint32_t NUM_SLOTS = SLOT_COLUMNS * SLOT_ROWS;
    static const glm::vec2 SLOT_SIZE = glm::vec2(0.15f);
    static const float SLOT_PADDING = 0.025f;

    SelectionMenuEntity::SelectionMenuEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::SelectionMenu))
    {
        rootTransform.Anchor = AnchorMode::Center;
        rootTransform.Pivot = AnchorMode::Center;
        rootTransform.Scale = PANEL_SIZE;

        squareTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        //   backgroundMaterial->SetTexture("_SpriteTex", squareTexture.get());
        backgroundMaterial->SetColor("_TintColor", Rendering::CANVAS_COLOR);
        backgroundMaterial->SetVector("_Scale", glm::vec4(1));

        UITransform headerTransform{};
        headerTransform.Anchor = AnchorMode::TopMiddle;
        headerTransform.Pivot = AnchorMode::BottomMiddle;
        headerTransform.Scale = HEADER_SIZE;
        headerTransform.Position = HEADER_OFFSET;

        headerEntity = std::make_unique<UISubEntity>(headerTransform);
        rootTransform.BindChild(headerEntity->GetTransform());

        headerTexture = std::make_shared<Rendering::Texture2D>("UI/SelectionScreen/Tex_SelectionHeader");
        headerMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        headerMaterial->SetTexture("_SpriteTex", headerTexture.get());
        headerMaterial->SetColor("_TintColor", glm::vec4(1));
        headerMaterial->SetVector("_Scale", glm::vec4(1));
    }

    void SelectionMenuEntity::InitializeSlots()
    {
        std::shared_ptr<Rendering::Shader> slotShader = Rendering::Shader::Get("UI/SpriteDefault");
        slotEntities.reserve(NUM_SLOTS);
        slotMaterials.reserve(NUM_SLOTS);
        UITransform baseSlotTransform{};
        baseSlotTransform.Anchor = AnchorMode::TopLeft;
        baseSlotTransform.Pivot = AnchorMode::TopLeft;
        baseSlotTransform.Scale = SLOT_SIZE;
        baseSlotTransform.Position = glm::vec2(SLOT_PADDING, -SLOT_PADDING);

        for (uint32_t i = 0; i < NUM_SLOTS; i++)
        {
            uint32_t column = i % SLOT_COLUMNS;
            uint32_t row = i / SLOT_COLUMNS;

            UITransform slotTransform = baseSlotTransform;
            slotTransform.Position.x += (SLOT_SIZE.x + SLOT_PADDING) * column;
            slotTransform.Position.y += -(SLOT_SIZE.y + SLOT_PADDING) * row;

            std::unique_ptr<UISubEntity> slotEntity = std::make_unique<UISubEntity>(slotTransform);
            rootTransform.BindChild(slotEntity->GetTransform());
            slotEntities.push_back(std::move(slotEntity));

            std::shared_ptr<Rendering::Material> slotMaterial = std::make_shared<Rendering::Material>(slotShader);
            slotMaterial->SetTexture("_SpriteTex", squareTexture.get());
            slotMaterial->SetColor("_TintColor", glm::vec4(0, 0, 0, 1));
            slotMaterial->SetVector("_Scale", glm::vec4(1));

            slotMaterials.push_back(std::move(slotMaterial));
        }

        GetSelectionMenuManager()->InitializeSlots(slotEntities,
            slotMaterials,
            NUM_SLOTS);
    }
} // namespace Beer::System
