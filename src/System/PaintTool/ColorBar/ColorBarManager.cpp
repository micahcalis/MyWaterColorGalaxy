#include "System/PaintTool/ColorBar/ColorBarManager.hpp"
#include "ColorBarLevel.hpp"
#include "System/Base/Input/MouseInput.hpp"
#include "System/Components/Colliders/QuadCollider.hpp"
#include "System/Components/UI/UITransform.hpp"
#include "System/Delegates/BeerEvent.hpp"
#include <cstdlib>
#include <stdexcept>

namespace Beer::System
{
    static const float ANIMATION_SPEED = 3.0f;
    static const float OFFSET_SCALE = 0.05f;

    ColorBarManager::ColorBarManager(UITransform* colorBarTransform,
        Function<MouseInput> getMouseInput,
        Function<void> markQuadTreeDirty,
        Function<void> openColorPicker,
        Function<void, glm::vec4> setColorDisplayColor,
        Function<void, glm::vec4, ColorBarLevel> setGalaxyBufferColor,
        Function<std::array<glm::vec4, 4>> getGalaxyColors,
        BeerEvent<void(glm::vec4)>* onColorPicked,
        BeerEvent<void()>* onColorPickerClosed,
        BeerEvent<void()>* onNewSeed)
        : colorBarTransform(colorBarTransform), getMouseInput(getMouseInput), markQuadTreeDirty(markQuadTreeDirty), openColorPicker(openColorPicker), setColorDisplayColor(setColorDisplayColor), setGalaxyBufferColor(setGalaxyBufferColor), getGalaxyColors(getGalaxyColors), onColorPicked(onColorPicked), onColorPickerClosed(onColorPickerClosed), onNewSeed(onNewSeed)
    {
        onColorPickerClosed->Subscribe([this]() {
            currentController->Unsubscribe();
            currentController = nullptr;
        });

        onNewSeed->Subscribe([this]() -> void { SetGalaxyColorsFromSeed(); });
    }

    void ColorBarManager::Update()
    {
        AnimateColorLevels(ColorBarType::Planet);
        AnimateColorLevels(ColorBarType::Galaxy);
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

        controller->SetSubscriptions(subscribeToColorPicker, unsubscribeToColorPicker);
        controller->OnButtonClicked.Subscribe([this](ColorBarController* controller) -> void { TryOpenColorPicker(controller); });

        if (type == ColorBarType::Galaxy)
        {
            controller->OnNewColor.Subscribe(
                [this](glm::vec4 color, ColorBarLevel level) -> void { setGalaxyBufferColor(color, level); });
        }

        controllersMap[level] = std::move(controller);
    }

    void ColorBarManager::CreateAnimator(ColorBarType type)
    {
        auto& controllersMap = type == ColorBarType::Planet ? planetColorControllers : galaxyColorControllers;

        std::vector<UITransform*> transforms;
        transforms.reserve(controllersMap.size());

        for (int i = 0; i < controllersMap.size(); i++)
        {
            ColorBarLevel level = static_cast<ColorBarLevel>(i);
            transforms.push_back(controllersMap.at(level)->GetTransform());
        }

        auto& animator = type == ColorBarType::Planet ? planetLayersAnimator : galaxyLayersAnimator;
        animator = std::make_unique<StackAnimator>(colorBarTransform,
            transforms,
            ANIMATION_SPEED,
            OFFSET_SCALE);
    }

    void ColorBarManager::ForceSetColorsFromSeed()
    {
        SetGalaxyColorsFromSeed();
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

            for (int l = 0; l < 4; l++)
            {
                galaxyColorControllers[levels[l]]->SetColor(colors[l]);
            }
        }
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

    void ColorBarManager::AnimateColorLevels(ColorBarType type)
    {
        auto& animator = type == ColorBarType::Planet ? planetLayersAnimator : galaxyLayersAnimator;

        if (animator == nullptr)
            return;

        MouseInput input = getMouseInput();
        int selectedLayer = -1;

        auto& controllersMap = type == ColorBarType::Planet ? planetColorControllers : galaxyColorControllers;

        if (MouseInContainer(input.PixelPos))
        {
            for (int i = static_cast<int>(controllersMap.size()) - 1; i >= 0; i--)
            {
                ColorBarLevel levelToCheck = static_cast<ColorBarLevel>(i);
                const auto& controller = controllersMap.at(levelToCheck);

                if (QuadCollider::Hit(controller->GetTransform(), input.PixelPos))
                {
                    selectedLayer = i;
                    break;
                }
            }
        }

        animator->SetSelectedLayer(selectedLayer);
        bool layersDirty;
        animator->Update(layersDirty);

        if (layersDirty)
            markQuadTreeDirty();
    }

    void ColorBarManager::TryOpenColorPicker(ColorBarController* controller)
    {
        if (currentController != nullptr)
        {
            currentController->Unsubscribe();
        }

        currentController = controller;
        openColorPicker();
        setColorDisplayColor(currentController->GetColor());
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
} // namespace Beer::System

