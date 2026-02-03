#pragma once
#include "RHITypes.h"
#include "RHI.h"
#include <memory>

/**
 * Initialize the RHI module and register it with ModuleManager.
 * Must be called explicitly to ensure proper DLL loading and module registration.
 */
extern "C" RHI_API void InitializeModuleRHI();

namespace CarrotToy {
namespace RHI {

/**
 * RHI Subsystem - Manages RHI device initialization and access
 * This is the central point for RHI management within the RHI module
 */
class RHI_API RHISubsystem {
public:
    static RHISubsystem& Get();
    
    /**
     * Initialize the RHI subsystem with the given graphics API
     * @param api The graphics API to use (OpenGL, Vulkan, etc.)
     * @param loader Proc address loader for loading OpenGL functions (required for OpenGL)
     */
    bool Initialize(GraphicsAPI api, IRHIDevice::ProcAddressLoader loader);
    
    /**
     * Get the RHI device
     * Returns nullptr if not initialized
     */
    std::shared_ptr<IRHIDevice> GetDevice() const { return device; }
    
    /**
     * Check if RHI is initialized
     */
    bool IsInitialized() const { return initialized; }
    
    /**
     * Shutdown the RHI subsystem
     */
    void Shutdown();
    
private:
    RHISubsystem() = default;
    ~RHISubsystem() = default;
    RHISubsystem(const RHISubsystem&) = delete;
    RHISubsystem& operator=(const RHISubsystem&) = delete;
    
    bool initialized = false;
    std::shared_ptr<IRHIDevice> device;
    GraphicsAPI currentAPI = GraphicsAPI::OpenGL;
};

} // namespace RHI
} // namespace CarrotToy
