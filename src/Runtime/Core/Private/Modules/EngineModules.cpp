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
