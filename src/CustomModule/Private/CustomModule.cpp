/**
 * CustomModule - Application Module Implementation
 * 
 * This is a custom application built as a separate output target,
 * parallel to DefaultGame and TestRHIApp.
 * 
 * The main() entry point is provided by the Launch module.
 * This module uses IMPLEMENT_APPLICATION_MODULE to register itself
 * and create the global GEngineLoop instance.
 */

#include "CustomModule.h"
#include "Modules/Module.h"
#include "CoreUtils.h"

void FCustomModule::StartupModule()
{
    LOG("=== CustomModule Application Starting ===");
    LOG("=== CustomModule 应用程序启动 ===");
    LOG("CustomModule: Module startup");
    LOG("This demonstrates that CustomModule is now a separate application");
    LOG("parallel to Runtime, DefaultGame, and TestRHIApp.");
    
    // Custom initialization logic here
    DoSomething();
}

void FCustomModule::ShutdownModule()
{
    LOG("CustomModule: Module shutdown");
    LOG("=== CustomModule Application Complete ===");
}

void FCustomModule::DoSomething()
{
    LOG("CustomModule: Doing something awesome!");
    LOG("CustomModule is running successfully as an application module!");
}

// Register the custom module as an application module
// This creates the GEngineLoop instance and registers the module
IMPLEMENT_APPLICATION_MODULE(FCustomModule, CustomModule, "CustomModuleProject")
