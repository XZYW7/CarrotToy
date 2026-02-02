#include "Modules/EngineModules.h"
#include "CoreUtils.h"

// Core Engine Module Implementation
void FCoreEngineModule::StartupModule()
{
	LOG("CoreEngineModule: Startup");
	LOG("CoreEngineModule: Initializing core engine systems");
	// Initialize core systems: memory allocators, file system, logging, etc.
}

void FCoreEngineModule::ShutdownModule()
{
	LOG("CoreEngineModule: Shutdown");
	LOG("CoreEngineModule: Shutting down core engine systems");
	// Clean up core systems
}

// RHI Module Implementation
void FRHIModule::StartupModule()
{
	LOG("RHIModule: Startup - Initializing RHI subsystem");
	// RHI initialization happens when devices are created via createRHIDevice()
	// This module just manages the lifecycle
}

void FRHIModule::ShutdownModule()
{
	LOG("RHIModule: Shutdown - Shutting down RHI subsystem");
	// Cleanup any global RHI state if needed
}
