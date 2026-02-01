# CarrotToy Module System

## Overview

CarrotToy implements a module management mechanism inspired by Unreal Engine, providing:
- **Code Loading**: Dynamic and static module loading
- **Plugin Management**: Plugin discovery and lifecycle management
- **Project Separation**: Distinct management for Engine and Game modules
- **Application Modules**: Special module type for standalone applications

## Architecture

### Module Types

The system supports four module types (`EModuleType`):

1. **Engine Modules**: Core engine functionality (rendering, physics, etc.)
2. **Game Modules**: Game-specific logic and content
3. **Plugin Modules**: Optional, loadable extensions
4. **Application Modules**: Application entry points for specialized tasks

### Module Lifecycle

Modules follow a defined lifecycle:

```
Register → Startup → Running → Shutdown → Unload
```

#### Key Methods

- `StartupModule()`: Called after module is loaded, initialize resources
- `ShutdownModule()`: Called before unload, cleanup resources
- `PreUnloadCallback()`: Called before shutdown (for hot-reload scenarios)
- `PostLoadCallback()`: Called after reload (for hot-reload scenarios)

### Loading Phases

Modules can be loaded at different phases (`ELoadingPhase`):

- `PreInit`: Before application initialization
- `Default`: Normal loading phase
- `PostDefault`: After default modules
- `PostEngineInit`: After engine initialization

## Usage

### Implementing a Module

#### 1. Define Module Interface

```cpp
// MyModule.h
#pragma once
#include "Modules/ModuleInterface.h"

class FMyModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual bool IsGameModule() const override { return false; }
};
```

#### 2. Implement Module

```cpp
// MyModule.cpp
#include "MyModule.h"
#include "CoreUtils.h"

void FMyModule::StartupModule()
{
    LOG("MyModule: Startup - Initializing custom systems");
    // Initialize your systems here
}

void FMyModule::ShutdownModule()
{
    LOG("MyModule: Shutdown - Cleaning up");
    // Cleanup your systems here
}
```

#### 3. Register Module

```cpp
// At the end of MyModule.cpp or in a separate registration file
#include "Modules/Module.h"

// For an Engine module:
IMPLEMENT_MODULE(FMyModule, MyModule)

// For a Game module:
IMPLEMENT_GAME_MODULE(FMyModule, MyModule)
```

### Loading Modules

```cpp
// Load a module
FModuleManager::Get().LoadModule("MyModule");

// Get a module instance
IModuleInterface* module = FModuleManager::Get().GetModule("MyModule");

// Check if loaded
bool isLoaded = FModuleManager::Get().IsModuleLoaded("MyModule");

// Unload a module
FModuleManager::Get().UnloadModule("MyModule");
```

### Creating an Application Module

Application modules are special modules that serve as application entry points:

```cpp
// TestApp.h
#pragma once
#include "Modules/ModuleInterface.h"

class FTestAppModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
    void RunTests();  // Custom application logic
};
```

```cpp
// TestApp.cpp
#include "TestApp.h"
#include "Modules/Module.h"

void FTestAppModule::StartupModule()
{
    LOG("TestApp: Starting");
    RunTests();
}

void FTestAppModule::ShutdownModule()
{
    LOG("TestApp: Shutting down");
}

void FTestAppModule::RunTests()
{
    LOG("TestApp: Running application logic");
    // Your application-specific code here
}

// Register as Application module
IMPLEMENT_APPLICATION_MODULE(FTestAppModule, TestApp, "TestApplication")
```

## Plugin System

### Plugin Discovery

```cpp
// Discover plugins in a directory
FModuleManager::Get().DiscoverPlugins("/path/to/plugins");

// Get available plugins
TArray<FPluginDescriptor> plugins = FModuleManager::Get().GetAvailablePlugins();

// Load a plugin
FModuleManager::Get().LoadPlugin("MyPlugin");

// Unload a plugin
FModuleManager::Get().UnloadPlugin("MyPlugin");
```

### Plugin Descriptor

Plugins are described using `FPluginDescriptor`:

```cpp
FPluginDescriptor descriptor;
descriptor.PluginName = "MyPlugin";
descriptor.FriendlyName = "My Awesome Plugin";
descriptor.Version = "1.0.0";
descriptor.Description = "Does something amazing";
descriptor.CreatedBy = "Your Name";
descriptor.bEnabledByDefault = true;

// Add modules to plugin
FModuleDescriptor moduleDesc;
moduleDesc.ModuleName = "MyPluginModule";
moduleDesc.Type = EModuleType::Plugin;
descriptor.Modules.Add(moduleDesc);
```

## Example: RHI Test Application

The project includes a `FTestApplicationModule` as an example Application module for RHI testing:

```cpp
// Create an instance
FTestApplicationModule* testApp = 
    static_cast<FTestApplicationModule*>(
        FModuleManager::Get().GetModule("TestApplication")
    );

// Initialize RHI test environment
testApp->InitializeRHITest();

// Run tests
testApp->RunRHITests();

// Cleanup
testApp->ShutdownRHITest();
```

## Module Organization

### Directory Structure

```
src/
├── Runtime/
│   ├── Core/                    # Core engine module
│   │   ├── Public/
│   │   │   └── Modules/
│   │   │       ├── ModuleInterface.h
│   │   │       ├── Module.h
│   │   │       ├── ModuleDescriptor.h
│   │   │       ├── EngineModules.h
│   │   │       └── TestApplicationModule.h
│   │   └── Private/
│   │       ├── Modules/
│   │       │   ├── Module.cpp
│   │       │   ├── EngineModules.cpp
│   │       │   └── TestApplicationModule.cpp
│   │       └── CoreModule.cpp
│   └── Launch/                  # Application launcher
│       └── ...
└── DefaultGame/                 # Game project
    └── Private/
        ├── GameModules.h
        ├── GameModules.cpp
        └── DefualtGame.cpp
```

## Best Practices

1. **Module Dependencies**: Declare dependencies explicitly to ensure proper load order
2. **Shutdown Order**: Modules shut down in reverse order (App → Game → Plugin → Engine)
3. **Stateless Startup**: Don't rely on global state during `StartupModule()`
4. **Resource Management**: Always cleanup in `ShutdownModule()`
5. **Hot Reload**: Support `PreUnloadCallback()` and `PostLoadCallback()` for hot-reloading
6. **Module Type**: Choose the appropriate module type:
   - Use `Engine` for engine core functionality
   - Use `Game` for game-specific logic
   - Use `Plugin` for optional extensions
   - Use `Application` for standalone applications

## Advanced Features

### Module Queries

```cpp
// Get all modules of a specific type
TArray<FName> engineModules = 
    FModuleManager::Get().GetModulesByType(EModuleType::Engine);

TArray<FName> gameModules = 
    FModuleManager::Get().GetModulesByType(EModuleType::Game);
```

### Dependency Management

Modules can declare dependencies:

```cpp
FModuleDescriptor desc;
desc.ModuleName = "MyModule";
desc.Dependencies.Add("CoreEngine");
desc.Dependencies.Add("RHI");
```

Dependencies are automatically loaded before the dependent module.

## Extending the System

### Adding Custom Module Types

1. Extend `EModuleType` enum in `ModuleDescriptor.h`
2. Update shutdown logic in `FModuleManager::ShutdownAll()`
3. Create registration macros in `Module.h`

### Dynamic Loading (Advanced)

For dynamic library loading:

```cpp
// Load a module from a dynamic library
FModuleManager::Get().LoadModuleDynamic("/path/to/module.dll");
```

Note: Dynamic loading requires proper symbol export and module factories.

## Testing

The system includes a test application module (`FTestApplicationModule`) that demonstrates:
- Application module lifecycle
- RHI test framework integration
- Custom module logic

To use:
```cpp
FModuleManager::Get().LoadModule("TestApplication");
auto* testModule = static_cast<FTestApplicationModule*>(
    FModuleManager::Get().GetModule("TestApplication")
);
testModule->RunRHITests();
```

## Troubleshooting

### Module Not Found
- Ensure module is registered with `IMPLEMENT_MODULE` or variants
- Check that module source file is compiled and linked
- Verify module name matches registration

### Load Order Issues
- Declare dependencies explicitly
- Use appropriate `ELoadingPhase`
- Check module startup logs

### Shutdown Crashes
- Implement proper cleanup in `ShutdownModule()`
- Don't access other modules during shutdown
- Release resources in reverse acquisition order

## Future Enhancements

- [ ] JSON-based plugin descriptors (.uplugin files)
- [ ] Hot-reloading support
- [ ] Module versioning and compatibility checks
- [ ] Asynchronous module loading
- [ ] Module dependency graph visualization
- [ ] Per-module configuration files
