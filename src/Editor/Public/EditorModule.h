#pragma once
#include "Modules/Module.h"
#include "EditorAPI.h"
#include "MaterialEditor.h"

/**
 * Editor Module
 * Handles editor-specific functionality like material editor, UI tools, etc.
 */
namespace CarrotToy {
class EDITOR_API FEditorModule : public IModuleInterface
{
public:
	virtual ~FEditorModule() override = default;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool IsGameModule() const override { return false; }

	std::unique_ptr<MaterialEditor> CreateEditor(class Renderer* renderer);
	
};

extern "C" EDITOR_API void InitializeModuleEditor();
}
