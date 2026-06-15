#pragma once

namespace Beer::System
{
    enum class InputMode
    {
        MouseKeyboard,
        PenDisplay
    };

    class Input
    {
        friend class InputManager;

    private:
        static inline InputMode inputMode = InputMode::MouseKeyboard;

    public:
        static InputMode Mode() { return inputMode; }
    };
} // namespace Beer::System
