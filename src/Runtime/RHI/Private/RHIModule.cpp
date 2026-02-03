#include "Modules/Module.h"
#include "Modules/ModuleInterface.h"
#include "CoreUtils.h"
#include "RHI/RHITypes.h"
#include "RHI/RHIModuleInit.h"
#include "RHI/RHI.h"
#include <iostream>

namespace CarrotToy {
namespace RHI {

// RHISubsystem implementation
RHISubsystem& RHISubsystem::Get() {
    static RHISubsystem instance;
    return instance;
}

bool RHISubsystem::Initialize(GraphicsAPI api, IRHIDevice::ProcAddressLoader loader) {
    if (initialized) {
        LOG("RHISubsystem: Already initialized");
        return true;
    }
    
    LOG("RHISubsystem: Initializing with API: " << static_cast<int>(api));
    
    // Create RHI device
    device = createRHIDevice(api);
    if (!device) {
        std::cerr << "RHISubsystem: Failed to create RHI device" << std::endl;
        return false;
    }
    
    // Initialize device with proc address loader (required for OpenGL cross-DLL)
    if (!device->initialize(loader)) {
        std::cerr << "RHISubsystem: Failed to initialize RHI device" << std::endl;
        device.reset();
        return false;
    }
    
    // Set as global device for convenience
    setGlobalDevice(device);
    
    currentAPI = api;
    initialized = true;
    LOG("RHISubsystem: Initialized successfully");
    return true;
}

void RHISubsystem::Shutdown() {
    if (!initialized) {
        return;
    }
    
    LOG("RHISubsystem: Shutting down...");
    
    if (device) {
        device->shutdown();
        device.reset();
    }
    
    // Clear global device
    setGlobalDevice(nullptr);
    
    initialized = false;
    LOG("RHISubsystem: Shutdown complete");
}

} // namespace RHI
} // namespace CarrotToy

/**
 * RHI (Rendering Hardware Interface) Module
 * Handles graphics API abstraction and device management
 */
class FRHIModule : public IModuleInterface
{
public:
	virtual ~FRHIModule() override = default;

	virtual void StartupModule() override
	{
		LOG("RHIModule: Startup - RHI subsystem ready (device creation deferred)");
		// RHI device initialization happens via RHISubsystem::Initialize()
		// which should be called after Platform is initialized with a valid OpenGL context
	}

	virtual void ShutdownModule() override
	{
		LOG("RHIModule: Shutdown - Shutting down RHI subsystem");
		CarrotToy::RHI::RHISubsystem::Get().Shutdown();
	}
	
	virtual bool IsGameModule() const override { return false; }
};

// Register RHI Module using RHI_API export macro
IMPLEMENT_MODULE_WITH_API(FRHIModule, RHI, RHI_API)
