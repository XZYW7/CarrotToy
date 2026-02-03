#include "EditorModule.h"
#include "CoreUtils.h"
#include "EditorAPI.h"

namespace CarrotToy {
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
std::unique_ptr<MaterialEditor> FEditorModule::CreateEditor(Renderer* renderer) {
	auto editor = std::make_unique<MaterialEditor>();
	if(editor->initialize(renderer)) return editor;
	return nullptr;
}

IMPLEMENT_MODULE_WITH_API(FEditorModule, Editor, EDITOR_API)

}