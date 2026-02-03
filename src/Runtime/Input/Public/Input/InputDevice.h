#pragma once

#include "InputTypes.h"
#include <memory>

#ifndef INPUT_API
    #if defined(_WIN32) || defined(_WIN64)
        #ifdef INPUT_BUILD_SHARED
            #define INPUT_API __declspec(dllexport)
        #elif defined(INPUT_IMPORT_SHARED)
            #define INPUT_API __declspec(dllimport)
        #else
            #define INPUT_API
        #endif
    #else
        #define INPUT_API
    #endif
#endif

namespace CarrotToy {
namespace Platform {
    class IPlatformWindow;
}

namespace Input {

// Input device interface - abstracts input polling from a window
class IInputDevice {
public:
    virtual ~IInputDevice() = default;
    
    // Mouse input
    virtual CursorPosition getCursorPosition() const = 0;
    virtual bool isMouseButtonPressed(MouseButton button) const = 0;
    virtual bool isMouseButtonDown(MouseButton button) const = 0;
    virtual bool isMouseButtonReleased(MouseButton button) const = 0;
    
    // Keyboard input (basic support)
    virtual bool isKeyPressed(KeyCode key) const = 0;
    virtual bool isKeyDown(KeyCode key) const = 0;
    virtual bool isKeyReleased(KeyCode key) const = 0;
    
    // Update input state (call once per frame)
    virtual void update() = 0;
};

// Factory function to create input device for a window
INPUT_API std::shared_ptr<IInputDevice> createInputDevice(std::shared_ptr<Platform::IPlatformWindow> window);

} // namespace Input
} // namespace CarrotToy
