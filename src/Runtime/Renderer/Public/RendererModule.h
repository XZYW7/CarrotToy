#pragma once
#include "Modules/ModuleInterface.h"
#include "RendererAPI.h"

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

/**
 * Initialize the Renderer module and register it with ModuleManager.
 * Must be called explicitly to ensure proper DLL loading and module registration.
 */
extern "C" RENDERER_API void InitializeModuleRenderer();
