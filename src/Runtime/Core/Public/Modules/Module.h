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

// Implement a module (default to Engine type)
#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
    /** Global registrant object for this module when linked statically */ \
    static IModuleInterface* CreateModule##ModuleName() { return new ModuleImplClass(); } \
    static FStaticallyLinkedModuleRegistrant< ModuleImplClass > ModuleRegistrant##ModuleName( TEXT(#ModuleName), EModuleType::Engine );


// Implement an application module (with application entry point)
#define IMPLEMENT_APPLICATION_MODULE( ModuleImplClass, ModuleName, GameName ) \
    /** For monolithic builds, we must statically define the game's name string */ \
    TCHAR GInternalProjectName[64] = TEXT( GameName ); \
    /** Global registrant object for this application module */ \
    static IModuleInterface* CreateModule##ModuleName() { return new ModuleImplClass(); } \
    static FStaticallyLinkedModuleRegistrant< ModuleImplClass > ModuleRegistrant##ModuleName( TEXT(#ModuleName), EModuleType::Application ); \
    FMainLoop GEngineLoop;

// Legacy macro for backward compatibility
#define IMPLEMENT_APPLICATION( ModuleName, GameName ) \
    IMPLEMENT_APPLICATION_MODULE(FDefaultModule, ModuleName, GameName)
 
// Implement a game module
#define IMPLEMENT_GAME_MODULE( ModuleName, GameName ) \
    IMPLEMENT_APPLICATION_MODULE( FDefaultModule, ModuleName, GameName )
    
