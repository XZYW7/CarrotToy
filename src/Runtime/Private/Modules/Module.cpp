#include "Modules/Module.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

ModuleManager& ModuleManager::Get()
{
    static ModuleManager mgr;
    return mgr;
}

void ModuleManager::RegisterModule(const FName& name, FUniquePtr<IModuleInterface> module)
{
    LOG("ModuleManager: "<< "Registering module " << name);
    modules[name] = std::move(module);
}

IModuleInterface* ModuleManager::GetModule(const FName& name)
{
    auto it = modules.find(name);
    return it == modules.end() ? nullptr : it->second.get();
}

void ModuleManager::UnloadModule(const FName& name)
{
    auto it = modules.find(name);
    if (it != modules.end()) {
        it->second->ShutdownModule();
        modules.erase(it);
    }
}

void ModuleManager::ShutdownAll()
{
    for (auto& kv : modules) kv.second->ShutdownModule();
    modules.clear();
}



#pragma region Unvalidated

// TODO : Not validated
bool ModuleManager::LoadModuleDynamic(const FName& name)
{
    // FString path = GetModuleDiskPath(name);
    FString path = name; // For testing, assume name is full path
#ifdef _WIN32
    HMODULE lib = LoadLibraryA(path.c_str());
    if (!lib) { std::cerr << "LoadLibrary failed: " << path << "\n"; return false; }
    using CreateFn = IModuleInterface* (*)();
    CreateFn create = (CreateFn)GetProcAddress(lib, "CreateModule");
    if (!create) { std::cerr << "CreateModule not found: " << path << "\n"; return false; }
#else
    void* lib = dlopen(path.c_str(), RTLD_NOW);
    if (!lib) { std::cerr << "dlopen failed: " << dlerror() << "\n"; return false; }
    using CreateFn = IModuleInterface* (*)();
    CreateFn create = (CreateFn)dlsym(lib, "CreateModule");
    if (!create) { std::cerr << "CreateModule not found: " << dlerror() << "\n"; return false; }
#endif

    IModuleInterface* mod = create();
    if (!mod) { std::cerr << "CreateModule returned null: " << path << "\n"; return false; }
    mod->StartupModule();
    RegisterModule(name, FUniquePtr<IModuleInterface>(mod));
    return true;
}
#pragma endregion