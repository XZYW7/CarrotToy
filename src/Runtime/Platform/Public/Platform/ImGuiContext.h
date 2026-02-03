#pragma once

#include <memory>
#include "Platform.h"
namespace CarrotToy {

// Forward declarations
namespace Platform {
    class IPlatformWindow;
}

/**
 * ImGuiContext - Manages ImGui lifecycle and backend integration
 * 
 * This class encapsulates ImGui initialization, rendering, and cleanup
 * to decouple ImGui from specific platform and renderer backends.
 */
class ImGuiContext {
public:
    virtual ~ImGuiContext() = default;
    
    /**
     * Initialize ImGui with the given window
     * @param window Platform window for input and rendering
     * @return true if initialization succeeded
     */
    virtual bool initialize(Platform::IPlatformWindow* window) = 0;
    
    /**
     * Shutdown ImGui and cleanup resources
     */
    virtual void shutdown() = 0;
    
    /**
     * Begin a new ImGui frame
     * Call this before any ImGui rendering commands
     */
    virtual void beginFrame() = 0;
    
    /**
     * End the current ImGui frame and render
     * Call this after all ImGui rendering commands
     */
    virtual void endFrame() = 0;
    
    /**
     * Check if ImGui is initialized
     */
    virtual bool isInitialized() const = 0;
};

/**
 * Factory function to create ImGui context
 * Currently creates an OpenGL3+GLFW backend
 */
PLATFORM_API std::shared_ptr<ImGuiContext> createImGuiContext();

} // namespace CarrotToy
