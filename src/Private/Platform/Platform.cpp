#include "Platform.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

namespace CarrotToy {
namespace Platform {

// GLFW-based platform implementation (works on Windows, Linux, macOS)
class GLFWPlatformWindow : public IPlatformWindow {
public:
    explicit GLFWPlatformWindow(GLFWwindow* window) 
        : window_(window), resizeCallback_(nullptr) {
        // Validate window parameter - caller should ensure this is valid
        if (!window_) {
            std::cerr << "Error: GLFWPlatformWindow created with null window" << std::endl;
            return;
        }
        
        // Store this pointer in window user data for callbacks
        glfwSetWindowUserPointer(window_, this);
        
        // Set up resize callback
        glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);
    }
    
    ~GLFWPlatformWindow() override {
        if (window_) {
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }
    }
    
    bool shouldClose() const override {
        return glfwWindowShouldClose(window_);
    }
    
    void setShouldClose(bool value) override {
        glfwSetWindowShouldClose(window_, value ? GLFW_TRUE : GLFW_FALSE);
    }
    
    void setTitle(const char* title) override {
        glfwSetWindowTitle(window_, title);
    }
    
    void getSize(uint32_t& width, uint32_t& height) const override {
        int w, h;
        glfwGetWindowSize(window_, &w, &h);
        width = static_cast<uint32_t>(w);
        height = static_cast<uint32_t>(h);
    }
    
    void setSize(uint32_t width, uint32_t height) override {
        glfwSetWindowSize(window_, static_cast<int>(width), static_cast<int>(height));
    }
    
    void getFramebufferSize(uint32_t& width, uint32_t& height) const override {
        int w, h;
        glfwGetFramebufferSize(window_, &w, &h);
        width = static_cast<uint32_t>(w);
        height = static_cast<uint32_t>(h);
    }
    
    void show() override {
        glfwShowWindow(window_);
    }
    
    void hide() override {
        glfwHideWindow(window_);
    }
    
    void focus() override {
        glfwFocusWindow(window_);
    }
    
    void makeContextCurrent() override {
        glfwMakeContextCurrent(window_);
    }
    
    void swapBuffers() override {
        glfwSwapBuffers(window_);
    }
    
    void* getProcAddress(const char* name) override {
        return reinterpret_cast<void*>(glfwGetProcAddress(name));
    }
    
    WindowHandle getNativeHandle() const override {
        return window_;
    }
    
    void setResizeCallback(ResizeCallback callback) override {
        resizeCallback_ = callback;
    }
    
private:
    GLFWwindow* window_;
    ResizeCallback resizeCallback_;
    
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        auto* platformWindow = static_cast<GLFWPlatformWindow*>(glfwGetWindowUserPointer(window));
        if (platformWindow && platformWindow->resizeCallback_) {
            platformWindow->resizeCallback_(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
    }
};

// GLFW-based platform implementation
class GLFWPlatform : public IPlatform {
public:
    GLFWPlatform() : initialized_(false) {}
    
    ~GLFWPlatform() override {
        shutdown();
    }
    
    bool initialize() override {
        if (initialized_) {
            return true;
        }
        
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        initialized_ = true;
        return true;
    }
    
    void shutdown() override {
        if (initialized_) {
            glfwTerminate();
            initialized_ = false;
        }
    }
    
    PlatformType getPlatformType() const override {
        return getCurrentPlatformType();
    }
    
    std::shared_ptr<IPlatformWindow> createWindow(const WindowDesc& desc) override {
        if (!initialized_) {
            std::cerr << "Platform not initialized" << std::endl;
            return nullptr;
        }
        
        // Set window hints based on descriptor
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);
        
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        
        GLFWmonitor* monitor = desc.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
        GLFWwindow* window = glfwCreateWindow(
            desc.width, 
            desc.height, 
            desc.title, 
            monitor, 
            nullptr
        );
        
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return nullptr;
        }
        
        // Make context current and configure vsync if requested
        glfwMakeContextCurrent(window);
        if (desc.vsync) {
            glfwSwapInterval(1);
        }
        
        return std::make_shared<GLFWPlatformWindow>(window);
    }
    
    void pollEvents() override {
        glfwPollEvents();
    }
    
    std::vector<DisplayInfo> getDisplays() const override {
        std::vector<DisplayInfo> displays;
        
        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        
        for (int i = 0; i < count; ++i) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
            const char* monitorName = glfwGetMonitorName(monitors[i]);
            // glfwGetMonitorName can return null for disconnected monitors
            displays.emplace_back(
                mode->width,
                mode->height,
                mode->refreshRate,
                monitorName ? monitorName : "Unknown Monitor"
            );
        }
        
        return displays;
    }
    
    DisplayInfo getPrimaryDisplay() const override {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        const char* monitorName = glfwGetMonitorName(monitor);
        
        return DisplayInfo(
            mode->width,
            mode->height,
            mode->refreshRate,
            monitorName ? monitorName : "Primary Monitor"
        );
    }
    
    double getTime() const override {
        return glfwGetTime();
    }
    
private:
    bool initialized_;
};

// Factory function implementation
std::shared_ptr<IPlatform> createPlatform() {
    return std::make_shared<GLFWPlatform>();
}

// Platform type detection
PlatformType getCurrentPlatformType() {
#if defined(_WIN32) || defined(_WIN64)
    return PlatformType::Windows;
#elif defined(__APPLE__) && defined(__MACH__)
    return PlatformType::MacOS;
#elif defined(__linux__)
    return PlatformType::Linux;
#else
    return PlatformType::Unknown;
#endif
}

} // namespace Platform
} // namespace CarrotToy
