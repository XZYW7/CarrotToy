#include "Modules/Module.h"
#include <iostream>
#include <filesystem>

FModuleManager& FModuleManager::Get()
{
    static FModuleManager mgr;
    return mgr;
}

void FModuleManager::RegisterModule(const FName& name, FUniquePtr<IModuleInterface> module, EModuleType type)
{
    LOG("ModuleManager: Registering module " << name << " of type " << static_cast<int>(type));
    
    FModuleInfo info;
    info.ModuleInstance = std::move(module);
    info.Descriptor.ModuleName = name;
    info.Descriptor.Type = type;
    info.bIsLoaded = false;
    
    modules[name] = std::move(info);
}

IModuleInterface* FModuleManager::GetModule(const FName& name)
{
    auto it = modules.find(name);
    return it == modules.end() ? nullptr : it->second.ModuleInstance.get();
}

bool FModuleManager::IsModuleLoaded(const FName& name) const
{
    auto it = modules.find(name);
    return it != modules.end() && it->second.bIsLoaded;
}

bool FModuleManager::LoadModule(const FName& name)
{
    auto it = modules.find(name);
    if (it == modules.end()) {
        LOG("ModuleManager: Module " << name << " not found in registry");
        return false;
    }
    
    if (it->second.bIsLoaded) {
        LOG("ModuleManager: Module " << name << " already loaded");
        return true;
    }
    
    LOG("ModuleManager: Starting up module " << name);
    
    // Load dependencies first
    for (const auto& dep : it->second.Descriptor.Dependencies) {
        if (!IsModuleLoaded(dep)) {
            if (!LoadModule(dep)) {
                LOG("ModuleManager: Failed to load dependency " << dep << " for module " << name);
                return false;
            }
        }
    }
    
    // Startup the module
    it->second.ModuleInstance->StartupModule();
    it->second.bIsLoaded = true;
    
    LOG("ModuleManager: Module " << name << " loaded successfully");
    return true;
}

void FModuleManager::UnloadModule(const FName& name)
{
    auto it = modules.find(name);
    if (it != modules.end()) {
        if (it->second.bIsLoaded) {
            it->second.ModuleInstance->ShutdownModule();
            it->second.bIsLoaded = false;
        }
        modules.erase(it);
    }
}

void FModuleManager::ShutdownAll()
{
    LOG("ModuleManager: Shutting down all modules");
    
    // Shutdown in reverse order (game modules first, then engine modules)
    TArray<FName> engineModules;
    TArray<FName> gameModules;
    TArray<FName> pluginModules;
    TArray<FName> appModules;
    
    for (auto& kv : modules) {
        if (!kv.second.bIsLoaded) continue;
        
        switch (kv.second.Descriptor.Type) {
            case EModuleType::Application:
                appModules.Add(kv.first);
                break;
            case EModuleType::Game:
                gameModules.Add(kv.first);
                break;
            case EModuleType::Plugin:
                pluginModules.Add(kv.first);
                break;
            case EModuleType::Engine:
                engineModules.Add(kv.first);
                break;
        }
    }
    
    // Shutdown in reverse order: App -> Game -> Plugin -> Engine
    for (const auto& name : appModules) {
        modules[name].ModuleInstance->ShutdownModule();
        modules[name].bIsLoaded = false;
    }
    for (const auto& name : gameModules) {
        modules[name].ModuleInstance->ShutdownModule();
        modules[name].bIsLoaded = false;
    }
    for (const auto& name : pluginModules) {
        modules[name].ModuleInstance->ShutdownModule();
        modules[name].bIsLoaded = false;
    }
    for (const auto& name : engineModules) {
        modules[name].ModuleInstance->ShutdownModule();
        modules[name].bIsLoaded = false;
    }
    
    modules.clear();
}

void FModuleManager::DiscoverPlugins(const FString& pluginDirectory)
{
    LOG("ModuleManager: Discovering plugins in " << pluginDirectory);
    
    // TODO: In a real implementation, this would scan for .uplugin files
    // For now, we'll just log the directory
    // In the future, you could parse JSON plugin descriptors here
    
    if (!std::filesystem::exists(pluginDirectory)) {
        LOG("ModuleManager: Plugin directory does not exist: " << pluginDirectory);
        return;
    }
    
    // Example: scan for subdirectories that might be plugins
    try {
        for (const auto& entry : std::filesystem::directory_iterator(pluginDirectory)) {
            if (entry.is_directory()) {
                FName pluginName = entry.path().filename().string();
                if (availablePlugins.find(pluginName) == availablePlugins.end()) {
                    FPluginDescriptor desc(pluginName);
                    desc.FriendlyName = pluginName;
                    availablePlugins[pluginName] = desc;
                    LOG("ModuleManager: Discovered plugin " << pluginName);
                }
            }
        }
    } catch (const std::exception& e) {
        LOG("ModuleManager: Error discovering plugins: " << e.what());
    }
}

bool FModuleManager::LoadPlugin(const FName& pluginName)
{
    auto it = availablePlugins.find(pluginName);
    if (it == availablePlugins.end()) {
        LOG("ModuleManager: Plugin " << pluginName << " not found");
        return false;
    }
    
    LOG("ModuleManager: Loading plugin " << pluginName);
    
    // Load all modules in the plugin
    TArray<FName> loadedModules;
    for (const auto& moduleDesc : it->second.Modules) {
        if (LoadModule(moduleDesc.ModuleName)) {
            loadedModules.Add(moduleDesc.ModuleName);
        } else {
            LOG("ModuleManager: Failed to load module " << moduleDesc.ModuleName << " from plugin " << pluginName);
            // Unload previously loaded modules from this plugin
            for (const auto& loadedMod : loadedModules) {
                UnloadModule(loadedMod);
            }
            return false;
        }
    }
    
    loadedPluginModules[pluginName] = loadedModules;
    LOG("ModuleManager: Plugin " << pluginName << " loaded successfully");
    return true;
}

void FModuleManager::UnloadPlugin(const FName& pluginName)
{
    auto it = loadedPluginModules.find(pluginName);
    if (it == loadedPluginModules.end()) {
        LOG("ModuleManager: Plugin " << pluginName << " is not loaded");
        return;
    }
    
    LOG("ModuleManager: Unloading plugin " << pluginName);
    
    // Unload all modules from this plugin
    for (const auto& moduleName : it->second) {
        UnloadModule(moduleName);
    }
    
    loadedPluginModules.erase(it);
}

TArray<FPluginDescriptor> FModuleManager::GetAvailablePlugins() const
{
    TArray<FPluginDescriptor> result;
    for (const auto& kv : availablePlugins) {
        result.Add(kv.second);
    }
    return result;
}

TArray<FName> FModuleManager::GetModulesByType(EModuleType type) const
{
    TArray<FName> result;
    for (const auto& kv : modules) {
        if (kv.second.Descriptor.Type == type) {
            result.Add(kv.first);
        }
    }
    return result;
}




#pragma region Unvalidated

// TODO : Not validated
bool FModuleManager::LoadModuleDynamic(const FName& name)
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