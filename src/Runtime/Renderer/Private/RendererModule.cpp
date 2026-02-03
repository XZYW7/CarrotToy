#include "RendererModule.h"
#include "Modules/Module.h"
#include "CoreUtils.h"
#include "RendererAPI.h"

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

// Register Renderer Module using RENDERER_API export macro
IMPLEMENT_MODULE_WITH_API(FRendererModule, Renderer, RENDERER_API)
