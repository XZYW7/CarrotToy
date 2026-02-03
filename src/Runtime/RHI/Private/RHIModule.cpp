#include "Modules/Module.h"
#include "Modules/ModuleInterface.h"
#include "CoreUtils.h"
#include "RHI/RHITypes.h"

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
		LOG("RHIModule: Startup - Initializing RHI subsystem");
		// RHI initialization happens when devices are created via createRHIDevice()
		// This module just manages the lifecycle
	}

	virtual void ShutdownModule() override
	{
		LOG("RHIModule: Shutdown - Shutting down RHI subsystem");
		// Cleanup any global RHI state if needed
	}
	
	virtual bool IsGameModule() const override { return false; }
};

// Register RHI Module using RHI_API export macro
IMPLEMENT_MODULE_WITH_API(FRHIModule, RHI, RHI_API)
