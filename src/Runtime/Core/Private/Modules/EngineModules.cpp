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

// Force DLL loading function
extern "C" void ForceCoreModuleLoad()
{
	// This function exists solely to be referenced from applications
	// to ensure the Core DLL is loaded, triggering global constructors
	// that register the module with FModuleManager
}
