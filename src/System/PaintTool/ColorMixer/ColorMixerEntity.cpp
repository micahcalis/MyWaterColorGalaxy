#include "ColorMixerEntity.hpp"
#include "PigmentButton.hpp"
#include "Rendering/Texture/Texture2D.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Default/UI/QuadTreeEntity.hpp"
#include "System/PaintTool/PaintToolContext.hpp"
#include "glm/fwd.hpp"
#include <print>

namespace Beer::System
{
    const static uint32_t NUM_PIGMENTS = 8;
    const static uint32_t NUM_ROWS = 2;

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
        float scale = 0.2f;
        pigmentTexture = std::make_shared<Rendering::Texture2D>("UI/General/Tex_SquareSprite");
        pigmentEntities.reserve(NUM_PIGMENTS);
        pigmentMaterials.reserve(NUM_PIGMENTS);

        for (int i = 0; i < NUM_PIGMENTS; i++)
        {
            UITransform transform{};
            transform.Anchor = AnchorMode::TopRight;
            transform.Pivot = AnchorMode::BottomRight;
            transform.Scale = glm::vec2(scale, scale);

            float offsetX = static_cast<float>(i % rowCount);
            float offsetY = static_cast<float>(i / rowCount);
            transform.Position = glm::vec2(-offsetX * scale, offsetY * scale);

            pigmentEntities.emplace_back(transform);
            rootTransform.BindChild(pigmentEntities[i].GetTransform());

            std::shared_ptr<Rendering::Material> material = std::make_shared<Rendering::Material>("UI/SpriteDefault");
            material->SetColor("_TintColor", GetPigmentColor(static_cast<PigmentType>(i)));
            material->SetTexture("_SpriteTex", pigmentTexture.get());
            material->SetVector("_Scale", glm::vec4(1, 1, 0, 0));
            pigmentMaterials.push_back(std::move(material));

            mixerManager->AddPigmentButton(pigmentEntities[i].GetTransform(),
                pigmentMaterials[i].get(),
                static_cast<PigmentType>(i));
        }
    }
} // namespace Beer::System
