# Module System Quick Reference

## Module Types
- `EModuleType::Engine` - Core engine functionality (Core, RHI, Editor, etc.)
- `EModuleType::Game` - **Deprecated** - Use Application type instead
- `EModuleType::Plugin` - Optional loadable extensions
- `EModuleType::Application` - Application entry points (one per executable)

## Registration Macros

```cpp
// Engine module (recommended for engine subsystems)
IMPLEMENT_MODULE(FMyModule, MyModule)

// Game module (DEPRECATED - use IMPLEMENT_APPLICATION_MODULE instead)
// IMPLEMENT_GAME_MODULE(FMyGameModule, MyGameModule)

// Application module (recommended for executable targets)
IMPLEMENT_APPLICATION_MODULE(FMyAppModule, MyApp, "MyAppName")
```

## Common Operations

```cpp
// Load module
FModuleManager::Get().LoadModule("ModuleName");

// Get module
auto* mod = FModuleManager::Get().GetModule("ModuleName");

// Check if loaded
bool loaded = FModuleManager::Get().IsModuleLoaded("ModuleName");

// Unload module
FModuleManager::Get().UnloadModule("ModuleName");

// Shutdown all
FModuleManager::Get().ShutdownAll();

// Query by type
auto engineMods = FModuleManager::Get().GetModulesByType(EModuleType::Engine);

// Plugin operations
FModuleManager::Get().DiscoverPlugins("/path/to/plugins");
FModuleManager::Get().LoadPlugin("PluginName");
FModuleManager::Get().UnloadPlugin("PluginName");
```

## Module Interface

```cpp
class IModuleInterface
{
    virtual void StartupModule() {}
    virtual void ShutdownModule() {}
    virtual void PreUnloadCallback() {}
    virtual void PostLoadCallback() {}
    virtual bool SupportsDynamicReloading() { return true; }
    virtual bool SupportsAutomaticShutdown() { return true; }
    virtual bool IsGameModule() const { return false; }
};
```

## Module Lifecycle

```
[Register] → StartupModule() → [Running] → ShutdownModule() → [Unload]
```

## Shutdown Order

Application → Plugin → Engine

(Note: Game module type is deprecated)
