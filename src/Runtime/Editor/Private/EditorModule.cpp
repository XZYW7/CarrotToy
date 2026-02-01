#include "EditorModule.h"
#include "CoreUtils.h"
#include "Modules/Module.h"

void FEditorModule::StartupModule()
{
	LOG("EditorModule: Startup");
	LOG("EditorModule: Initializing editor systems");
	// Initialize editor systems: material editor, UI tools, etc.
}

void FEditorModule::ShutdownModule()
{
	LOG("EditorModule: Shutdown");
	LOG("EditorModule: Shutting down editor systems");
	// Clean up editor systems
}

// Register the Editor module
IMPLEMENT_MODULE(FEditorModule, Editor)
