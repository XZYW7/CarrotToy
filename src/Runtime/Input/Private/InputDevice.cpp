#include "Input/InputDevice.h"
#include "Platform/Platform.h"
#include <unordered_map>

namespace CarrotToy {
namespace Input {

// Platform-based input device implementation
class PlatformInputDevice : public IInputDevice {
public:
    explicit PlatformInputDevice(std::shared_ptr<Platform::IPlatformWindow> window)
        : window_(window) {
    }
    
    CursorPosition getCursorPosition() const override {
        CursorPosition pos;
        if (window_) {
            window_->getCursorPos(pos.x, pos.y);
        } else {
            pos.x = 0.0;
            pos.y = 0.0;
        }
        return pos;
    }
    
    bool isMouseButtonPressed(MouseButton button) const override {
        if (!window_) return false;
        int buttonCode = static_cast<int>(button);
        return window_->getMouseButton(buttonCode);
    }
    
    bool isMouseButtonDown(MouseButton button) const override {
        return isMouseButtonPressed(button);
    }
    
    bool isMouseButtonReleased(MouseButton button) const override {
        return !isMouseButtonPressed(button);
    }
    
    bool isKeyPressed(KeyCode key) const override {
        // Basic implementation - can be expanded
        return false;
    }
    
    bool isKeyDown(KeyCode key) const override {
        return false;
    }
    
    bool isKeyReleased(KeyCode key) const override {
        return false;
    }
    
    void update() override {
        // Update input state tracking if needed
        // For now, we poll directly from the window
    }
    
private:
    std::shared_ptr<Platform::IPlatformWindow> window_;
};

// Factory function implementation
std::shared_ptr<IInputDevice> createInputDevice(std::shared_ptr<Platform::IPlatformWindow> window) {
    return std::make_shared<PlatformInputDevice>(window);
}

} // namespace Input
} // namespace CarrotToy
