#include "Platform/PlatformModule.h"
#include "Platform/PlatformModuleInit.h"
#include "Modules/Module.h"
#include "CoreUtils.h"
#include <iostream>

namespace CarrotToy {
namespace Platform {

// PlatformSubsystem implementation
PlatformSubsystem& PlatformSubsystem::Get() {
    static PlatformSubsystem instance;
    return instance;
}

bool PlatformSubsystem::Initialize() {
    if (initialized) {
        LOG("PlatformSubsystem: Already initialized");
        return true;
    }
    
    LOG("PlatformSubsystem: Initializing...");
    
    // Create platform context - manages GLFW initialization
    platformContext = createPlatformContext();
    if (!platformContext) {
        std::cerr << "PlatformSubsystem: Failed to create platform context" << std::endl;
        return false;
    }
    
    // Initialize GLFW
    if (!platformContext->initializePlatform()) {
        std::cerr << "PlatformSubsystem: Failed to initialize platform (GLFW)" << std::endl;
        return false;
    }
    
    // Create platform instance
    platform = createPlatform();
    if (!platform) {
        std::cerr << "PlatformSubsystem: Failed to create platform" << std::endl;
        return false;
    }
    
    // Note: Platform's initialize() is skipped because platformContext already initialized GLFW
    // GLFWPlatform::initialize() in Platform.cpp detects this by calling glfwGetPrimaryMonitor()
    // and reuses the existing GLFW context instead of calling glfwInit() again
    
    initialized = true;
    LOG("PlatformSubsystem: Initialized successfully");
    return true;
}

std::shared_ptr<IPlatformWindow> PlatformSubsystem::CreatePlatformWindow(const WindowDesc& desc) {
    if (!initialized) {
        std::cerr << "PlatformSubsystem: Cannot create window - subsystem not initialized" << std::endl;
        return nullptr;
    }
    
    LOG("PlatformSubsystem: Creating window: " << desc.title);
    auto window = platform->createWindow(desc);
    if (!window) {
        std::cerr << "PlatformSubsystem: Failed to create window" << std::endl;
        return nullptr;
    }
    
    return window;
}

bool PlatformSubsystem::InitializeGraphicsContext(IPlatformWindow* window) {
    if (!initialized) {
        std::cerr << "PlatformSubsystem: Cannot initialize graphics context - subsystem not initialized" << std::endl;
        return false;
    }
    
    if (!window) {
        std::cerr << "PlatformSubsystem: Cannot initialize graphics context - window is null" << std::endl;
        return false;
    }
    
    LOG("PlatformSubsystem: Initializing graphics context (GLAD) for window");
    return platformContext->initializeGraphicsContext(window);
}

PlatformContext::ProcAddressLoader PlatformSubsystem::GetProcAddressLoader() const {
    if (!platformContext) {
        std::cerr << "PlatformSubsystem: Cannot get proc address loader - platform context not initialized" << std::endl;
        return nullptr;
    }
    return platformContext->getProcAddressLoader();
}

void PlatformSubsystem::PollEvents() {
    if (platform) {
        platform->pollEvents();
    }
}

void PlatformSubsystem::Shutdown() {
    if (!initialized) {
        return;
    }
    
    LOG("PlatformSubsystem: Shutting down...");
    
    // Shutdown in reverse order
    platform.reset();
    
    if (platformContext) {
        platformContext->shutdownPlatform();
        platformContext.reset();
    }
    
    initialized = false;
    LOG("PlatformSubsystem: Shutdown complete");
}

} // namespace Platform
} // namespace CarrotToy

// Platform module implementation
class FPlatformModule : public IModuleInterface {
public:
    virtual ~FPlatformModule() override = default;
    
    virtual void StartupModule() override {
        LOG("PlatformModule: Startup - Initializing Platform subsystem");
        CarrotToy::Platform::PlatformSubsystem::Get().Initialize();
    }
    
    virtual void ShutdownModule() override {
        LOG("PlatformModule: Shutdown - Shutting down Platform subsystem");
        CarrotToy::Platform::PlatformSubsystem::Get().Shutdown();
    }
    
    virtual bool IsGameModule() const override { return false; }
};

// Register Platform Module using PLATFORM_API export macro
IMPLEMENT_MODULE_WITH_API(FPlatformModule, Platform, PLATFORM_API)
