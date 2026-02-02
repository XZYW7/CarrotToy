#include "RendererModule.h"
#include "Modules/Module.h"
#include "CoreUtils.h"

// Renderer Module Implementation
void FRendererModule::StartupModule()
{
	LOG("RendererModule: Startup - Initializing renderer subsystem");
	// Renderer initialization
	// Material system, shader management, etc.
}

void FRendererModule::ShutdownModule()
{
	LOG("RendererModule: Shutdown - Shutting down renderer subsystem");
	// Cleanup renderer resources
}

// Force DLL loading function
extern "C" void ForceRendererModuleLoad()
{
	// This function exists solely to be referenced from applications
	// to ensure the Renderer DLL is loaded, triggering global constructors
	// that register the module with FModuleManager
}

// Register Renderer Module
IMPLEMENT_MODULE(FRendererModule, Renderer)
