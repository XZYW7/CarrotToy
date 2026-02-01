#pragma once
#include "Modules/ModuleInterface.h"

/**
 * Test Application Module for RHI Testing
 * This module demonstrates how to create an Application-type module
 * that can be used for testing RHI implementations
 */
class FTestApplicationModule : public IModuleInterface
{
public:
	virtual ~FTestApplicationModule() override = default;

	/**
	 * Called right after the module has been loaded
	 * Initialize application-specific resources here
	 */
	virtual void StartupModule() override;

	/**
	 * Called before the module is unloaded
	 * Clean up application-specific resources here
	 */
	virtual void ShutdownModule() override;

	/**
	 * Returns whether this is a game module
	 */
	virtual bool IsGameModule() const override
	{
		return true; // Application modules are typically considered game modules
	}

	/**
	 * Initialize RHI test environment
	 */
	void InitializeRHITest();

	/**
	 * Run RHI test suite
	 */
	void RunRHITests();

	/**
	 * Clean up RHI test environment
	 */
	void ShutdownRHITest();

private:
	bool bRHITestInitialized = false;
};
