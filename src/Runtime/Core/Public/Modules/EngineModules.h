#pragma once
#include "Modules/ModuleInterface.h"

/**
 * Core Engine Module
 * Handles core engine functionality like memory management,
 * file system, logging, and other fundamental systems
 */
class FCoreEngineModule : public IModuleInterface
{
public:
	virtual ~FCoreEngineModule() override = default;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool IsGameModule() const override { return false; }
};

/**
 * Initialize the Core module and register it with ModuleManager.
 * Must be called explicitly to ensure proper DLL loading and module registration.
 */
extern "C" CORE_API void InitializeModuleCoreEngine();
