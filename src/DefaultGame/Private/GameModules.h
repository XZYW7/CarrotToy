#pragma once
#include "Modules/ModuleInterface.h"

/**
 * Game Module
 * Contains game-specific logic and functionality
 */
class FGameModule : public IModuleInterface
{
public:
	virtual ~FGameModule() override = default;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool IsGameModule() const override { return true; }
};

/**
 * Gameplay Module
 * Handles gameplay systems like player controller, game mode, etc.
 */
class FGameplayModule : public IModuleInterface
{
public:
	virtual ~FGameplayModule() override = default;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool IsGameModule() const override { return true; }
};
