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
 * Force Core module DLL to be loaded by referencing this function.
 * This ensures the module's global constructors run and IMPLEMENT_MODULE registers it.
 */
extern "C" void ForceCoreModuleLoad();
