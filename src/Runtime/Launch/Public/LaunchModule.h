#pragma once
#include "Modules/ModuleInterface.h"

/**
 * Launch Module
 * Handles application entry point, main loop, and application lifecycle
 * This is typically statically linked to ensure the entry point is available
 */
class FLaunchModule : public IModuleInterface
{
public:
	virtual ~FLaunchModule() override = default;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool IsGameModule() const override { return false; }
};
