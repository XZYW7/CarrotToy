#pragma once
#include "Modules/ModuleInterface.h"

/**
 * Default Game Application Module
 * Main entry point for the CarrotToy default game/editor application
 */
class FDefaultGameModule : public IModuleInterface
{
public:
	virtual ~FDefaultGameModule() override = default;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool IsGameModule() const override { return false; }
};
