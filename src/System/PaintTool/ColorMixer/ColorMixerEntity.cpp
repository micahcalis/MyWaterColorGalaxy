#include "ColorMixerEntity.hpp"
#include "PigmentButton.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/General/Transform.hpp"
#include "System/Components/UI/UISubEntity.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "glm/fwd.hpp"

namespace Beer::System
{
    const static uint32_t NUM_PIGMENTS = 8;
    const static uint32_t NUM_ROWS = 2;
    const static float PIGMENT_BUTTON_SCALE = 0.2f;
    const static float CLEAR_BUTTON_SCALE = 0.25f;

    ColorMixerEntity::ColorMixerEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::PaintTool))
    {
        colorMixerDisplayMat = std::make_shared<Rendering::Material>("UI/ColorMixerSprite");
        colorMixerDisplayMat->SetColor("_CanvasColor", glm::vec4(0.969f, 0.969f, 0.914, 1));

        rootTransform.Anchor = AnchorMode::Center;
        rootTransform.Pivot = AnchorMode::Center;
        MarkDirty();
    }

    void ColorMixerEntity::InitializePigments()
    {
        ColorMixerManager* mixerManager = GetMixerManager();

        uint32_t rowCount = NUM_PIGMENTS / NUM_ROWS;
        squareTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        pigmentEntities.reserve(NUM_PIGMENTS);
        pigmentMaterials.reserve(NUM_PIGMENTS);

        for (int i = 0; i < NUM_PIGMENTS; i++)
        {
            UITransform transform{};
            transform.Anchor = AnchorMode::TopRight;
            transform.Pivot = AnchorMode::BottomRight;
            transform.Scale = glm::vec2(PIGMENT_BUTTON_SCALE, PIGMENT_BUTTON_SCALE);

            float offsetX = static_cast<float>(i % rowCount);
            float offsetY = static_cast<float>(i / rowCount);
            transform.Position = glm::vec2(-offsetX * PIGMENT_BUTTON_SCALE, offsetY * PIGMENT_BUTTON_SCALE);

            pigmentEntities.emplace_back(std::make_unique<UISubEntity>(transform));
            rootTransform.BindChild(pigmentEntities[i]->GetTransform());

            std::shared_ptr<Rendering::Material> material = std::make_shared<Rendering::Material>("UI/SpriteDefault");
            material->SetColor("_TintColor", GetPigmentColor(static_cast<PigmentType>(i)));
            material->SetTexture("_SpriteTex", squareTexture.get());
            material->SetVector("_Scale", glm::vec4(1, 1, 0, 0));
            pigmentMaterials.push_back(std::move(material));

            mixerManager->AddPigmentButton(pigmentEntities[i]->GetTransform(),
                pigmentMaterials[i].get(),
                static_cast<PigmentType>(i));
        }
    }

    void ColorMixerEntity::InitializeClearButton(Function<void> markCanvasClear)
    {
        ColorMixerManager* mixerManager = GetMixerManager();

        UITransform baseClearTransform{};
        baseClearTransform.Scale = glm::vec2(CLEAR_BUTTON_SCALE, CLEAR_BUTTON_SCALE);
        baseClearTransform.Pivot = AnchorMode::BottomRight;
        baseClearTransform.Anchor = AnchorMode::BottomLeft;

        clearButtonBg = std::make_unique<UISubEntity>(baseClearTransform);
        rootTransform.BindChild(clearButtonBg->GetTransform());

        UITransform iconClearTransform{};
        iconClearTransform.Scale = glm::vec2(CLEAR_BUTTON_SCALE, CLEAR_BUTTON_SCALE);
        iconClearTransform.Pivot = AnchorMode::Center;
        iconClearTransform.Anchor = AnchorMode::Center;
        iconClearTransform.Depth += 0.01f;

        clearButtonIcon = std::make_unique<UISubEntity>(iconClearTransform);
        clearButtonBg->GetTransform()->BindChild(clearButtonIcon->GetTransform());

        clearBgMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        clearBgMaterial->SetColor("_TintColor", glm::vec4(1));
        clearBgMaterial->SetTexture("_SpriteTex", squareTexture.get());
        clearBgMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        clearIconTexture = std::make_shared<Rendering::Texture2D>("UI/ColorMixer/Tex_GarbageIcon");
        clearIconMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        clearIconMaterial->SetColor("_TintColor", glm::vec4(0.5f, 0.5f, 0.5f, 1));
        clearIconMaterial->SetTexture("_SpriteTex", clearIconTexture.get());
        clearIconMaterial->SetVector("_Scale", glm::vec4(1, 1, 0, 0));

        mixerManager->SetClearButton(markCanvasClear, clearButtonIcon->GetTransform(), clearIconMaterial.get());
    }
} // namespace Beer::System
