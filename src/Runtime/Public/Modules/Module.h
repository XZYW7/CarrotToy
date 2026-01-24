#pragma once
#include "CoreUtils.h"

#include "ModuleInterface.h"
#include "Luanch.h"

#if defined(_WIN32)
#include <windows.h>
#endif 

class FDefaultModule : public IModuleInterface {
public:

};

class FModuleManager {
public:
    static FModuleManager& Get();

    // register module instance (for static-linked modules or after dynamic factory)
    void RegisterModule(const FName& name, FUniquePtr<IModuleInterface> module);
    IModuleInterface* GetModule(const FName& name);
    void UnloadModule(const FName& name);
    void ShutdownAll();

    bool LoadModule(const FName& name) {
        return false;
    }
    // dynamic load from path (uses platform layer)
    // FModuleManager::instance().LoadModuleDynamic("path/to/GameModule.dll")
    bool LoadModuleDynamic(const FString& path);
    
private:
    FModuleManager() = default;
    FMap<FName, FUniquePtr<IModuleInterface>> modules;
};


template< class ModuleClass >
class FStaticallyLinkedModuleRegistrant
{
public:
    FStaticallyLinkedModuleRegistrant(const FName& InModuleName)
    {
        FModuleManager::Get().RegisterModule(InModuleName, FUniquePtr<IModuleInterface>(CreateModule()));
    }

    // overload to accept literal/text (e.g. TEXT("ModuleName")) used by macros
    FStaticallyLinkedModuleRegistrant(const TCHAR* InModuleName)
    {
        FName Name(InModuleName);
        FModuleManager::Get().RegisterModule(Name, FUniquePtr<IModuleInterface>(CreateModule()));
    }

    static IModuleInterface* CreateModule()
    {
        return new ModuleClass();
    }
};
// If we're linking monolithically we assume all modules are linked in with the main binary.
#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
    /** Global registrant object for this module when linked statically */ \
    ModuleImplClass* CreateModule() { return new ModuleImplClass(); } \
    static FStaticallyLinkedModuleRegistrant< ModuleImplClass > ModuleRegistrant##ModuleName( TEXT(#ModuleName) );



#define IMPLEMENT_APPLICATION( ModuleName, GameName ) \
    /* For monolithic builds, we must statically define the game's name string (See Core.h) */ \
    TCHAR GInternalProjectName[64] = TEXT( GameName ); \
    IMPLEMENT_MODULE(FDefaultModule, ModuleName) \
    FMainLoop GEngineLoop; 
