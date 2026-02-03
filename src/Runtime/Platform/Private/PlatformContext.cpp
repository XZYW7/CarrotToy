#include "Platform/PlatformContext.h"
#include "Platform/Platform.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

namespace CarrotToy {
namespace Platform {

/**
 * GLFW-based implementation of PlatformContext
 */
class GLFWPlatformContext : public PlatformContext {
public:
    GLFWPlatformContext() 
        : platformInitialized_(false), graphicsContextInitialized_(false) {
    }
    
    ~GLFWPlatformContext() override {
        shutdownPlatform();
    }
    
    bool initializePlatform() override {
        if (platformInitialized_) {
            return true;
        }
        
        if (!glfwInit()) {
            std::cerr << "PlatformContext: Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        std::cout << "PlatformContext: GLFW initialized successfully" << std::endl;
        platformInitialized_ = true;
        return true;
    }
    
    void shutdownPlatform() override {
        if (platformInitialized_) {
            glfwTerminate();
            platformInitialized_ = false;
            graphicsContextInitialized_ = false;
            std::cout << "PlatformContext: GLFW terminated" << std::endl;
        }
    }
    
    bool initializeGraphicsContext(IPlatformWindow* window) override {
        if (!platformInitialized_) {
            std::cerr << "PlatformContext: Cannot initialize graphics context - platform not initialized" << std::endl;
            return false;
        }
        
        if (!window) {
            std::cerr << "PlatformContext: Cannot initialize graphics context - window is null" << std::endl;
            return false;
        }
        
        // Make the window's context current
        window->makeContextCurrent();
        
        // Create a proper function that wraps the proc address loading
        // This avoids lambda capture issues and ensures type safety
        struct LoaderContext {
            static void* loadProc(const char* name) {
                return reinterpret_cast<void*>(glfwGetProcAddress(name));
            }
        };
        
        if (!gladLoadGLLoader(LoaderContext::loadProc)) {
            std::cerr << "PlatformContext: Failed to initialize GLAD" << std::endl;
            return false;
        }
        
        // Verify OpenGL context is available
        const GLubyte* version = glGetString(GL_VERSION);
        if (!version) {
            std::cerr << "PlatformContext: OpenGL context not available" << std::endl;
            return false;
        }
        
        std::cout << "PlatformContext: GLAD initialized successfully" << std::endl;
        std::cout << "PlatformContext: OpenGL Version: " << version << std::endl;
        
        graphicsContextInitialized_ = true;
        return true;
    }
    
    ProcAddressLoader getProcAddressLoader() const override {
        // Return GLFW's proc address function directly
        // This is a static function with no captures, safe for function pointer conversion
        struct LoaderFunctor {
            static void* loadProc(const char* name) {
                return reinterpret_cast<void*>(glfwGetProcAddress(name));
            }
        };
        return LoaderFunctor::loadProc;
    }
    
    bool isPlatformInitialized() const override {
        return platformInitialized_;
    }
    
    bool isGraphicsContextInitialized() const override {
        return graphicsContextInitialized_;
    }
    
private:
    bool platformInitialized_;
    bool graphicsContextInitialized_;
};

// Factory function implementation
std::shared_ptr<PlatformContext> createPlatformContext() {
    return std::make_shared<GLFWPlatformContext>();
}

} // namespace Platform
} // namespace CarrotToy
