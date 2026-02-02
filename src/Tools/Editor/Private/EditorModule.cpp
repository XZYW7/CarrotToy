#include "EditorModule.h"
#include "CoreUtils.h"

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

// Note: Editor is not registered as a module - it's a tool library linked directly
