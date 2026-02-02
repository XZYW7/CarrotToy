#include "EditorModule.h"
#include "CoreUtils.h"
#include "EditorAPI.h"
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


IMPLEMENT_MODULE_WITH_API(FEditorModule, Editor, EDITOR_API)