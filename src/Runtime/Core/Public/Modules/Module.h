#pragma once
#include "CoreUtils.h"

#include "ModuleInterface.h"
#include "ModuleDescriptor.h"

#if defined(_WIN32)
#include <windows.h>
#endif 

class FDefaultModule : public IModuleInterface {
public:

};

/**
 * Module information wrapper
 */
struct FModuleInfo
{
	FUniquePtr<IModuleInterface> ModuleInstance;
	FModuleDescriptor Descriptor;
	bool bIsLoaded = false;
};

class FModuleManager {
public:
    static FModuleManager& Get();

    // Register module instance (for static-linked modules or after dynamic factory)
    void RegisterModule(const FName& name, FUniquePtr<IModuleInterface> module, 
                       EModuleType type = EModuleType::Engine);
    
    // Get module by name
    IModuleInterface* GetModule(const FName& name);
    
    // Check if module is loaded
    bool IsModuleLoaded(const FName& name) const;
    
    // Load module by name (searches in known module paths)
    bool LoadModule(const FName& name);
    
    // Unload a specific module
    void UnloadModule(const FName& name);
    
    // Shutdown all modules
    void ShutdownAll();

    // Dynamic load from path (uses platform layer)
    // FModuleManager::Get().LoadModuleDynamic("path/to/GameModule.dll")
    bool LoadModuleDynamic(const FString& path);
    
    // Plugin management
    void DiscoverPlugins(const FString& pluginDirectory);
    bool LoadPlugin(const FName& pluginName);
    void UnloadPlugin(const FName& pluginName);
    TArray<FPluginDescriptor> GetAvailablePlugins() const;
    
    // Get all loaded modules by type
    TArray<FName> GetModulesByType(EModuleType type) const;
    
private:
    FModuleManager() = default;
    FMap<FName, FModuleInfo> modules;
    FMap<FName, FPluginDescriptor> availablePlugins;
    FMap<FName, TArray<FName>> loadedPluginModules; // plugin name -> module names
};


template< class ModuleClass >
class FStaticallyLinkedModuleRegistrant
{
public:
    FStaticallyLinkedModuleRegistrant(const FName& InModuleName, EModuleType InModuleType = EModuleType::Engine)
    {
        FModuleManager::Get().RegisterModule(InModuleName, FUniquePtr<IModuleInterface>(CreateModule()), InModuleType);
    }

    // overload to accept literal/text (e.g. TEXT("ModuleName")) used by macros
    FStaticallyLinkedModuleRegistrant(const TCHAR* InModuleName, EModuleType InModuleType = EModuleType::Engine)
    {
        FName Name(InModuleName);
        FModuleManager::Get().RegisterModule(Name, FUniquePtr<IModuleInterface>(CreateModule()), InModuleType);
    }

    static IModuleInterface* CreateModule()
    {
        return new ModuleClass();
    }
};

// Implement a module with explicit API export macro (for dynamic/shared modules)
// API_MACRO should be the module's export macro (e.g., CORE_API, RHI_API, RENDERER_API)
// Creates an exported InitializeModule function that must be called explicitly
#define IMPLEMENT_MODULE_WITH_API( ModuleImplClass, ModuleName, API_MACRO ) \
    namespace { \
        static IModuleInterface* CreateModule##ModuleName() { return new ModuleImplClass(); } \
    } \
    extern "C" API_MACRO void InitializeModule##ModuleName() \
    { \
        static bool bInitialized = false; \
        if (!bInitialized) { \
            FModuleManager::Get().RegisterModule(TEXT(#ModuleName), \
                FUniquePtr<IModuleInterface>(CreateModule##ModuleName()), \
                EModuleType::Engine); \
            bInitialized = true; \
        } \
    }

// Implement a module (uses CORE_API by default for backward compatibility)
// For dynamic/shared modules, this creates an exported init function
#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
    IMPLEMENT_MODULE_WITH_API( ModuleImplClass, ModuleName, CORE_API )

// Implement a module with static registration (for statically linked modules)
// Uses global constructor for automatic registration before main()
#define IMPLEMENT_MODULE_STATIC( ModuleImplClass, ModuleName ) \
    namespace { \
        static IModuleInterface* CreateModule##ModuleName() { return new ModuleImplClass(); } \
        static FStaticallyLinkedModuleRegistrant< ModuleImplClass > ModuleRegistrant##ModuleName( TEXT(#ModuleName), EModuleType::Engine ); \
    }


// Implement an application module (with application entry point)
// Application modules are in the executable, so they use static registration
#define IMPLEMENT_APPLICATION_MODULE( ModuleImplClass, ModuleName, GameName ) \
    /** For monolithic builds, we must statically define the game's name string */ \
    TCHAR GInternalProjectName[64] = TEXT( GameName ); \
    /** Global registrant for application module (uses static registration) */ \
    namespace { \
        static IModuleInterface* CreateModule##ModuleName() { return new ModuleImplClass(); } \
        static FStaticallyLinkedModuleRegistrant< ModuleImplClass > ModuleRegistrant##ModuleName( TEXT(#ModuleName), EModuleType::Application ); \
    } \
    FMainLoop GEngineLoop;

// Legacy macro for backward compatibility
#define IMPLEMENT_APPLICATION( ModuleName, GameName ) \
    IMPLEMENT_APPLICATION_MODULE(FDefaultModule, ModuleName, GameName)
 
// Implement a game module
#define IMPLEMENT_GAME_MODULE( ModuleName, GameName ) \
    IMPLEMENT_APPLICATION_MODULE( FDefaultModule, ModuleName, GameName )
    
