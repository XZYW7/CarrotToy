#pragma once
#include "Modules/ModuleInterface.h"

/**
 * Renderer Module
 * Handles high-level rendering functionality, materials, shaders,
 * and rendering pipeline management
 */
class FRendererModule : public IModuleInterface
{
public:
	virtual ~FRendererModule() override = default;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool IsGameModule() const override { return false; }
};
