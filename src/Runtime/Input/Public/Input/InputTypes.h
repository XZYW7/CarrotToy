#pragma once

namespace CarrotToy {
namespace Input {

// Mouse button codes (matching GLFW conventions for compatibility)
enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7
};

// Keyboard key codes (simplified set - can be expanded as needed)
enum class KeyCode {
    Unknown = -1,
    Space = 32,
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    // Add more as needed
};

// Input action states
enum class InputAction {
    Release = 0,
    Press = 1,
    Repeat = 2
};

// Mouse cursor position
struct CursorPosition {
    double x;
    double y;
};

} // namespace Input
} // namespace CarrotToy
