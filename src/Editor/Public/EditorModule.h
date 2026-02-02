#pragma once
#include "Modules/Module.h"

/**
 * Editor Module
 * Handles editor-specific functionality like material editor, UI tools, etc.
 */
class FEditorModule : public IModuleInterface
{
public:
	virtual ~FEditorModule() override = default;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool IsGameModule() const override { return false; }
};
