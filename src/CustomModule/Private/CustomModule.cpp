/**
 * CustomModule - Main Application Entry Point
 * 
 * This is a custom application built as a separate output target,
 * parallel to DefaultGame and TestRHIApp.
 */

#include "CustomModule.h"
#include "Modules/ModuleManager.h"
#include "CoreUtils.h"
#include <iostream>

// Register the custom module
IMPLEMENT_GAME_MODULE(FCustomModule, CustomModule)

int main(int argc, char* argv[])
{
    LOG("=== CustomModule Application Starting ===");
    LOG("=== CustomModule 应用程序启动 ===");
    
    // Initialize the module manager
    FModuleManager& moduleManager = FModuleManager::Get();
    
    // Load core engine modules
    LOG("\n[1] Loading core engine modules:");
    moduleManager.LoadModule("CoreEngine");
    moduleManager.LoadModule("RHI");
    
    // Load the custom module
    LOG("\n[2] Loading CustomModule:");
    moduleManager.LoadModule("CustomModule");
    
    // Get and use the custom module
    LOG("\n[3] Using CustomModule:");
    IModuleInterface* moduleInterface = moduleManager.GetModule("CustomModule");
    if (moduleInterface)
    {
        auto* customModule = dynamic_cast<FCustomModule*>(moduleInterface);
        if (customModule)
        {
            customModule->DoSomething();
        }
    }
    else
    {
        LOG("ERROR: Failed to load CustomModule!");
        return 1;
    }
    
    // Query modules by type
    LOG("\n[4] Querying loaded modules:");
    const auto& engineModules = moduleManager.GetModulesByType(EModuleType::Engine);
    LOG("Engine modules:");
    for (const auto& modName : engineModules)
    {
        LOG("  - " << modName);
    }

    const auto& gameModules = moduleManager.GetModulesByType(EModuleType::Game);
    LOG("Game modules:");
    for (const auto& modName : gameModules)
    {
        LOG("  - " << modName);
    }
    
    LOG("\n[5] CustomModule is running successfully!");
    LOG("This demonstrates that CustomModule is now a separate application");
    LOG("parallel to Runtime, DefaultGame, and TestRHIApp.");
    
    // Cleanup
    LOG("\n[6] Shutting down modules...");
    moduleManager.UnloadModule("CustomModule");
    moduleManager.UnloadModule("RHI");
    moduleManager.UnloadModule("CoreEngine");
    
    LOG("\n=== CustomModule Application Complete ===");
    return 0;
}
