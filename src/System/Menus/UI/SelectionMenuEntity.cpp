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

    static const glm::vec2 PANEL_SIZE = glm::vec2(1.525f, 1.0f);

    static const uint32_t SLOT_COLUMNS = 8;
    static const uint32_t SLOT_ROWS = 5;
    static const uint32_t NUM_SLOTS = SLOT_COLUMNS * SLOT_ROWS;
    static const glm::vec2 SLOT_SIZE = glm::vec2(0.15f);
    static const float SLOT_PADDING = 0.025f;
    static const glm::vec2 SLOT_OFFSET = glm::vec2(0.05f, -0.05f);

    static const uint32_t COLOR_SEED = 9453;

    static constexpr uint32_t HashInt(uint32_t x)
    {
        x ^= x >> 16;
        x *= 0x85ebca6bu;
        x ^= x >> 13;
        x *= 0xc2b2ae35u;
        x ^= x >> 16;
        return x;
    }

    static const float MIN_SATURATION = 0.5f;
    static const float MAX_SATURATION = 1.0f;
    static const float MIN_VALUE = 0.8f;
    static const float MAX_VALUE = 1.0f;

    static constexpr glm::vec3 HSVtoRGB(glm::vec3 hsv)
    {
        glm::vec3 c = hsv;
        glm::vec4 k = glm::vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
        glm::vec3 p = glm::abs(glm::fract(glm::vec3(c.x) + glm::vec3(k.x, k.y, k.z)) * 6.0f - glm::vec3(k.w));
        return c.z * glm::mix(glm::vec3(k.x), glm::clamp(p - glm::vec3(k.x), 0.0f, 1.0f), c.y);
    }
    static constexpr float Remap(float x, glm::vec2 a, glm::vec2 b)
    {
        return b.x + (x - a.x) * (b.y - b.x) / (a.y - a.x);
    }

    static constexpr glm::vec4 TransformSlotColor(glm::vec4 base)
    {
        glm::vec2 saturationRange = glm::vec2(MIN_SATURATION, MAX_SATURATION);

        glm::vec2 valueRange = glm::vec2(MIN_VALUE, MAX_VALUE);

        base.g = Remap(base.g, glm::vec2(0, 1), saturationRange);
        base.b = Remap(base.b, glm::vec2(0, 1), valueRange);
        glm::vec3 rgb = HSVtoRGB(glm::vec3(base.r, base.g, base.b));
        return glm::vec4(rgb.r, rgb.g, rgb.b, base.a);
    }

    SelectionMenuEntity::SelectionMenuEntity()
        : QuadTreeEntity(UITransform(), RenderRegister::CreateRenderComponent<QuadTreeRenderComponent>(ContextType::SelectionMenu))
    {
        rootTransform.Anchor = AnchorMode::Center;
        rootTransform.Pivot = AnchorMode::Center;
        rootTransform.Scale = PANEL_SIZE;

        backgroundTexture = std::make_shared<Rendering::Texture2D>("UI/SelectionScreen/Tex_SelectionBackground");
        backgroundMaterial = std::make_shared<Rendering::Material>("UI/SpriteDefault");
        backgroundMaterial->SetTexture("_SpriteTex", backgroundTexture.get());
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
        auto nextFloat = [](uint32_t& state) -> float {
            state = HashInt(state);
            return static_cast<float>(state) / static_cast<float>(std::numeric_limits<uint32_t>::max());
        };

        auto nextColor = [&nextFloat](uint32_t& state) -> glm::vec4 {
            return TransformSlotColor(
                glm::vec4(nextFloat(state),
                    nextFloat(state),
                    nextFloat(state),
                    1.0f));
        };

        uint32_t slotState = COLOR_SEED;

        slotTexture = std::make_shared<Rendering::Texture2D>("UI/SelectionScreen/Tex_LightBulb");
        std::shared_ptr<Rendering::Shader> slotShader = Rendering::Shader::Get("UI/SpriteDefault");
        slotEntities.reserve(NUM_SLOTS);
        slotMaterials.reserve(NUM_SLOTS);
        UITransform baseSlotTransform{};
        baseSlotTransform.Anchor = AnchorMode::TopLeft;
        baseSlotTransform.Pivot = AnchorMode::TopLeft;
        baseSlotTransform.Scale = SLOT_SIZE;
        baseSlotTransform.Position = SLOT_OFFSET;

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
            slotMaterial->SetTexture("_SpriteTex", slotTexture.get());
            slotMaterial->SetColor("_TintColor", nextColor(slotState));
            slotMaterial->SetVector("_Scale", glm::vec4(1));

            slotMaterials.push_back(std::move(slotMaterial));
        }

        GetSelectionMenuManager()->InitializeSlots(slotEntities,
            slotMaterials,
            NUM_SLOTS);
    }
} // namespace Beer::System
