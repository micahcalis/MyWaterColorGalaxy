#include "System/PaintTool/ColorBar/ColorBarManager.hpp"
#include "ColorBarLevel.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include <print>
#include <stdexcept>

namespace Beer::System
{
    static const std::array<std::string, 4> P_DISPLAY_PROP = {
        "_PrimaryColor",
        "_SecondaryColor",
        "_TertiaryColor",
        "_QuaternaryColor"};

    static const std::array<std::string, 4> G_DISPLAY_PROP = {
        "_ColorR",
        "_ColorG",
        "_ColorB",
        "_ColorA"};

    static const float ANIMATION_SPEED = 3.0f;
    static const float OFFSET_SCALE = 0.05f;

    ColorBarManager::ColorBarManager(UITransform* colorBarTransform,
        Function<MouseInput> getMouseInput,
        Function<void> markQuadTreeDirty,
        Function<void, glm::vec4> setColorDisplayColor,
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor,
        Function<std::array<glm::vec4, 4>> getGalaxyColors,
        Function<Rendering::Texture2D*> getBrushTexture,
        Function<void> pickerSelectColor,
        BeerEvent<void()>* onColorPickerClosed,
        BeerEvent<void()>* onNewSeed)
        : colorBarTransform(colorBarTransform)
        , getMouseInput(getMouseInput)
        , markQuadTreeDirty(markQuadTreeDirty)
        , setColorDisplayColor(setColorDisplayColor)
        , setGalaxyBufferColor(setGalaxyBufferColor)
        , getGalaxyColors(getGalaxyColors)
        , getBrushTexture(getBrushTexture)
        , pickerSelectColor(pickerSelectColor)
        , onColorPickerClosed(onColorPickerClosed)
        , onNewSeed(onNewSeed)
    {
        onColorPickerClosed->Subscribe([this]() {
            currentController = nullptr;
            this->setColorDisplayColor(glm::vec4(1));
        });

        onNewSeed->Subscribe([this]() -> void { SetGalaxyColorsFromSeed(); });
    }

    void ColorBarManager::Update()
    {
    }

    void ColorBarManager::CreateColorBarController(ColorBarLevel level,
        ColorBarType type,
        UITransform* transform,
        Rendering::Material* material,
        glm::vec4 initialColor)
    {
        auto& controllersMap = type == ColorBarType::Planet ? planetColorControllers : galaxyColorControllers;

        if (controllersMap.contains(level))
        {
            throw std::runtime_error(std::format("Color Bar Level Already Initialized: {}", magic_enum::enum_name(level)));
        }

        std::unique_ptr<ColorBarController> controller = std::make_unique<ColorBarController>(transform,
            material,
            initialColor,
            level);

        Function<SubscriptionToken, Function<void, glm::vec4>> subscribeToColorPicker =
            [this](Function<void, glm::vec4> func) -> SubscriptionToken {
            return onColorPicked->Subscribe(func);
        };

        Function unsubscribeToColorPicker = [this](SubscriptionToken token) -> void {
            onColorPicked->Unsubscribe(token);
        };

        controller->OnButtonClicked.Subscribe([this](ColorBarController* controller) -> void { OnColorBarButtonPressed(controller); });

        if (type == ColorBarType::Galaxy)
        {
            controller->OnNewColor.Subscribe(
                [this](glm::vec4 color, ColorBarLevel level) -> void {
                    setGalaxyBufferColor(color, level);
                    UpdateDisplayMaterials();
                });
        } else
        {
            controller->OnNewColor.Subscribe(
                [this](glm::vec4 color, ColorBarLevel level) -> void {
                    UpdateDisplayMaterials();
                });
        }

        controllersMap[level] = std::move(controller);
    }

    void ColorBarManager::ForceSetColorsFromSeed()
    {
        SetGalaxyColorsFromSeed();
    }

    void ColorBarManager::TrySetColor(glm::vec4 color)
    {
        if (currentController == nullptr)
            return;

        currentController->SetColor(color);
        UpdateDisplayMaterials();
    }

    void ColorBarManager::ReloadFromSerialized(const SerializablePaintTool& serializedPaintTool)
    {
        const std::array<ColorBarType, 2> types = {
            ColorBarType::Planet,
            ColorBarType::Galaxy};

        const std::array<ColorBarLevel, 4> levels = {
            ColorBarLevel::Primary,
            ColorBarLevel::Secondary,
            ColorBarLevel::Tertiary,
            ColorBarLevel::Quaternary};

        for (int t = 0; t < 2; t++)
        {
            const auto& colors = types[t] == ColorBarType::Planet
                ? serializedPaintTool.ComponentColors
                : serializedPaintTool.GalaxyColors;

            auto& controllers = types[t] == ColorBarType::Planet
                ? planetColorControllers
                : galaxyColorControllers;

            for (int l = 0; l < 4; l++)
            {
                controllers[levels[l]]->SetColor(colors[l]);
            }
        }

        UpdateDisplayMaterials();
    }

    glm::vec4 ColorBarManager::GetBarColor(ColorBarLevel level, ColorBarType type) const
    {
        auto& controllersMap = type == ColorBarType::Planet ? planetColorControllers : galaxyColorControllers;

        if (!controllersMap.contains(level))
        {
            throw std::runtime_error(std::format("Color Bar Level Unitialized: {}, {}", magic_enum::enum_name(level), magic_enum::enum_name(type)));
        }

        return controllersMap.at(level)->GetColor();
    }

    std::vector<glm::vec4> ColorBarManager::GetColors(ColorBarType type) const
    {
        std::vector<glm::vec4> colors;
        uint32_t res = type == ColorBarType::Planet ? planetColorControllers.size() : galaxyColorControllers.size();
        colors.reserve(res);

        for (uint32_t i = 0; i < res; i++)
        {
            colors.push_back(GetBarColor(static_cast<ColorBarLevel>(i), type));
        }

        return colors;
    }

    bool ColorBarManager::MouseInContainer(glm::vec2 mousePos)
    {
        return QuadCollider::Hit(colorBarTransform, mousePos);
    }

    void ColorBarManager::OnColorBarButtonPressed(ColorBarController* controller)
    {
        currentController = controller;
        setColorDisplayColor(currentController->GetColor());

        if (selectSpriteTransform != nullptr)
        {
            controller->GetTransform()->BindChild(selectSpriteTransform);
        }

        pickerSelectColor();
        markQuadTreeDirty();
        OnColorClicked.Invoke(controller->GetColor());
    }

    void ColorBarManager::SetGalaxyColorsFromSeed()
    {
        const std::array<ColorBarLevel, 4> levels = {
            ColorBarLevel::Primary,
            ColorBarLevel::Secondary,
            ColorBarLevel::Tertiary,
            ColorBarLevel::Quaternary};

        std::array<glm::vec4, 4> colors = getGalaxyColors();

        for (int i = 0; i < 4; i++)
        {
            galaxyColorControllers[levels[i]]->SetColor(colors[i]);
        }
    }

    void ColorBarManager::UpdateDisplayMaterials()
    {
        if (planetDisplayMaterial != nullptr)
        {
            std::vector<glm::vec4> planetColors = GetColors(ColorBarType::Planet);

            for (int i = 0; i < P_DISPLAY_PROP.size(); i++)
            {
                planetDisplayMaterial->SetColor(P_DISPLAY_PROP[i], planetColors[i]);
            }

            planetDisplayMaterial->SetTexture("_SpriteTex", getBrushTexture(), false);
        }

        if (galaxyDisplayMaterial != nullptr)
        {
            std::vector<glm::vec4> galaxyColors = GetColors(ColorBarType::Galaxy);

            for (int i = 0; i < G_DISPLAY_PROP.size(); i++)
            {
                galaxyDisplayMaterial->SetColor(G_DISPLAY_PROP[i], galaxyColors[i]);
            }
        }
    }
} // namespace Beer::System

