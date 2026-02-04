/**
 * RenderBackendSandbox - Application Entry Point
 * 
 * This sandbox provides a testing environment for DX12 and Vulkan API implementations.
 * 
 * The main() entry point is provided by the Launch module.
 * This file uses IMPLEMENT_APPLICATION_MODULE to register the sandbox module
 * and create the global GEngineLoop instance.
 */

#include "Launch.h"
#include "Modules/Module.h"
#include "RenderBackendSandboxModule.h"

// Register the RenderBackendSandbox module as an application module
// This creates the GEngineLoop instance and registers the module
IMPLEMENT_APPLICATION_MODULE(FRenderBackendSandboxModule, RenderBackendSandbox, "RenderBackendSandbox")
