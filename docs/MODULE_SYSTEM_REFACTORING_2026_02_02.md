# Module System Refactoring - Implementation Guide

## Date: 2026-02-02

## Problem Statement (Original)

The CarrotToy engine had the following issues with its module system:

1. **Modules not being managed**: Despite splitting the engine into different modules (Editor, Core, Launch, Renderer, RHI, and application modules like CustomModule, DefaultGame, TestRHIAPP), these modules weren't actually being managed by ModuleManager. Errors showed:
   - "ModuleManager: Module CoreEngine not found in registry"
   - "ModuleManager: Module RHI not found in registry"

2. **RHI Module in wrong location**: The RHI module was implemented in Core module (CoreModule.cpp and EngineModules.h/cpp), which was architecturally incorrect.

3. **Missing Module implementations**: Renderer and Launch had no Module handling - they were just compiled as library files without proper module lifecycle management.

## Solution Overview

### 1. Moved RHI Module to RHI Directory ✅

**Problem**: FRHIModule was defined in `Core/Public/Modules/EngineModules.h` and implemented in `Core/Private/Modules/EngineModules.cpp`, then registered in `Core/Private/CoreModule.cpp`. This violated the separation of concerns.

**Solution**:
- Created `src/Runtime/RHI/Private/RHIModule.cpp` with:
  - FRHIModule class definition (inline)
  - Implementation of StartupModule() and ShutdownModule()
  - IMPLEMENT_MODULE(FRHIModule, RHI) registration

- Removed FRHIModule from:
  - `Core/Public/Modules/EngineModules.h` - removed class declaration
  - `Core/Private/Modules/EngineModules.cpp` - removed implementation
  - `Core/Private/CoreModule.cpp` - removed IMPLEMENT_MODULE registration

**Benefits**:
- RHI module is now self-contained in its own directory
- Core module only contains Core engine functionality
- Follows UE4/UE5 modular architecture principles

### 2. Created Renderer Module Implementation ✅

**Problem**: Renderer directory had no module lifecycle management - it was just a library.

**Solution**:
- Created `src/Runtime/Renderer/Public/RendererModule.h`:
  - FRendererModule class declaration
  - Inherits from IModuleInterface
  - Declares StartupModule() and ShutdownModule()

- Created `src/Runtime/Renderer/Private/RendererModule.cpp`:
  - Implementation of module lifecycle methods
  - IMPLEMENT_MODULE(FRendererModule, Renderer) registration

**Benefits**:
- Renderer can now be properly initialized and shut down
- Can add renderer subsystem initialization code
- Enables hot-reloading of renderer code (future)

### 3. Created Launch Module Implementation ✅

**Problem**: Launch directory had no module lifecycle management - it was just a static library with main().

**Solution**:
- Created `src/Runtime/Launch/Public/LaunchModule.h`:
  - FLaunchModule class declaration
  - Inherits from IModuleInterface
  - Declares StartupModule() and ShutdownModule()

- Created `src/Runtime/Launch/Private/LaunchModule.cpp`:
  - Implementation of module lifecycle methods
  - IMPLEMENT_MODULE(FLaunchModule, Launch) registration

**Benefits**:
- Launch subsystem can be properly initialized
- Consistent with other engine modules
- Better lifecycle management

### 4. Updated Module Loading in Launch.cpp ✅

**Problem**: Launch.cpp only tried to load CoreEngine and RHI modules, not Renderer or Launch.

**Solution**:
Modified `FMainLoop::LoadPreInitModules()` to load modules in the correct order:
```cpp
// Load core engine modules in order
FModuleManager::Get().LoadModule("CoreEngine");
FModuleManager::Get().LoadModule("Launch");
FModuleManager::Get().LoadModule("RHI");
FModuleManager::Get().LoadModule("Renderer");
```

**Benefits**:
- All engine modules are now loaded at startup
- Proper initialization order
- Modules can depend on each other

## Understanding the Module System

### Module Registration

Modules are registered using the `IMPLEMENT_MODULE` macro:

```cpp
IMPLEMENT_MODULE(FMyModule, MyModuleName)
```

This macro expands to:
1. A global static FStaticallyLinkedModuleRegistrant object
2. The object's constructor calls FModuleManager::Get().RegisterModule()
3. Registration happens at program startup (before main())

### Module Loading

After registration, modules must be explicitly loaded:

```cpp
FModuleManager::Get().LoadModule("MyModuleName");
```

This does the following:
1. **Check Registry**: Verifies the module is registered
2. **Load Dependencies**: Recursively loads any dependencies first
3. **Call StartupModule()**: Initializes the module
4. **Mark as Loaded**: Sets bIsLoaded = true

### Module Lifecycle (Following UE Design)

Based on Unreal Engine's module system, here's what happens after a module is registered:

#### 1. Registration Phase (Program Startup)
- Global static objects created by IMPLEMENT_MODULE
- Constructors run before main()
- FModuleManager::RegisterModule() is called
- Module is added to registry with metadata
- Module state: **Registered but not loaded**

#### 2. Loading Phase (PreInit/Init)
- FModuleManager::LoadModule() is called
- Dependencies are loaded first (recursive)
- IModuleInterface::StartupModule() is called
- Module can initialize subsystems, create objects, etc.
- Module state: **Loaded**

#### 3. Runtime Phase
- Module provides services via its interface
- Other modules can get references via GetModule()
- Module can be queried for capabilities
- Module state: **Running**

#### 4. Shutdown Phase (Exit)
- FModuleManager::ShutdownAll() or UnloadModule()
- Shutdown happens in reverse order: App → Game → Plugin → Engine
- IModuleInterface::ShutdownModule() is called
- Module cleans up resources, destroys objects
- Module state: **Unloaded**

### Module Types

The system supports four module types:

1. **Engine Modules** (EModuleType::Engine)
   - Core engine functionality
   - Examples: CoreEngine, RHI, Renderer, Launch
   - Loaded early in PreInit phase
   - Shut down last

2. **Application Modules** (EModuleType::Application)
   - One per executable
   - Examples: DefaultGame, TestRHIApp, CustomModule
   - Loaded first (via IMPLEMENT_APPLICATION_MODULE)
   - Shut down first

3. **Game Modules** (EModuleType::Game)
   - Gameplay-specific code
   - Can be hot-reloaded
   - Shut down before engine modules

4. **Plugin Modules** (EModuleType::Plugin)
   - Optional extensions
   - Dynamically discovered and loaded
   - Can be enabled/disabled at runtime

### Static vs Dynamic Linking

#### Static Linking (Launch)
- Launch is statically linked: `set_kind("static")` in xmake.lua
- Ensures entry point (main) is available
- Module registration via global constructors
- Requires whole-archive linking: `/WHOLEARCHIVE` (Windows), `--whole-archive` (Linux)

#### Dynamic Linking (Core, RHI, Renderer, Editor)
- Built as shared libraries by default: `set_kind("shared")` 
- Module registration happens when DLL/SO is loaded
- Loaded automatically via dependency chain (add_deps)
- Can be hot-reloaded (future feature)

### Build System Integration

#### Module Dependencies
```lua
-- Renderer depends on Core and RHI
add_deps("Core", "RHI")
```

#### Application Dependencies
```lua
-- DefaultGame depends on Renderer and Launch
add_deps("Renderer", "Launch")
```

This creates a transitive dependency chain:
```
DefaultGame → Renderer → Core
                      → RHI → Core
           → Launch → Renderer → ...
```

All dependencies are loaded at program startup.

### Linker Flags for Static Libraries

For static libraries, unused symbols (like module registration) may be stripped. Use:

**Windows:**
```lua
add_ldflags("/WHOLEARCHIVE:Launch.lib", {force = true})
```

**Linux:**
```lua
add_ldflags("-Wl,--whole-archive", {force = true})
add_ldflags("-Wl,--no-whole-archive", {force = true})
```

**macOS:**
```lua
add_ldflags("-Wl,-force_load", {force = true})
```

## Expected Behavior After These Changes

### Module Loading Output

When running any application (DefaultGame, TestRHIApp, CustomModule), you should see:

```
FMainLoop: Loading PreInit Modules
FMainLoop: Loading Application Module: <AppName>
ModuleManager: Registering module <AppName> of type 1
ModuleManager: Starting up module <AppName>
<AppName>Module: Startup
ModuleManager: Module <AppName> loaded successfully

ModuleManager: Registering module CoreEngine of type 0
ModuleManager: Starting up module CoreEngine
CoreEngineModule: Startup
CoreEngineModule: Initializing core engine systems
ModuleManager: Module CoreEngine loaded successfully

ModuleManager: Registering module Launch of type 0
ModuleManager: Starting up module Launch
LaunchModule: Startup - Initializing launch subsystem
ModuleManager: Module Launch loaded successfully

ModuleManager: Registering module RHI of type 0
ModuleManager: Starting up module RHI
RHIModule: Startup - Initializing RHI subsystem
ModuleManager: Module RHI loaded successfully

ModuleManager: Registering module Renderer of type 0
ModuleManager: Starting up module Renderer
RendererModule: Startup - Initializing renderer subsystem
ModuleManager: Module Renderer loaded successfully

FMainLoop: Loaded Application Modules:
  - <AppName>
FMainLoop: Loaded Engine Modules:
  - CoreEngine
  - Launch
  - RHI
  - Renderer
  - Editor (if linked)
FMainLoop: Loaded Game Modules:
  (none by default)
```

### No More "Module not found in registry" Errors

The errors:
- "ModuleManager: Module CoreEngine not found in registry"
- "ModuleManager: Module RHI not found in registry"

Should no longer appear because:
1. CoreEngine is registered in Core/Private/CoreModule.cpp
2. RHI is registered in RHI/Private/RHIModule.cpp
3. Renderer is registered in Renderer/Private/RendererModule.cpp
4. Launch is registered in Launch/Private/LaunchModule.cpp

All registration happens automatically via IMPLEMENT_MODULE macros before main() runs.

### Module Lifecycle Hooks Working

Each module now has proper lifecycle hooks:

1. **StartupModule()** - Called when module loads
   - Initialize subsystems
   - Register services
   - Create objects

2. **ShutdownModule()** - Called on exit
   - Cleanup resources
   - Unregister services
   - Destroy objects

## Files Changed

### Added
- `src/Runtime/RHI/Private/RHIModule.cpp` - RHI module registration
- `src/Runtime/Renderer/Public/RendererModule.h` - Renderer module interface
- `src/Runtime/Renderer/Private/RendererModule.cpp` - Renderer module implementation
- `src/Runtime/Launch/Public/LaunchModule.h` - Launch module interface
- `src/Runtime/Launch/Private/LaunchModule.cpp` - Launch module implementation

### Modified
- `src/Runtime/Core/Public/Modules/EngineModules.h` - Removed FRHIModule
- `src/Runtime/Core/Private/Modules/EngineModules.cpp` - Removed RHI implementation
- `src/Runtime/Core/Private/CoreModule.cpp` - Removed RHI registration
- `src/Runtime/Launch/Private/Launch.cpp` - Added Renderer/Launch module loading
- `src/DefaultGame/xmake.lua` - Added Linux linker flags

## Verification Steps

To verify the module system is working:

1. **Build the project**:
   ```bash
   xmake
   ```

2. **Run any application**:
   ```bash
   xmake run DefaultGame
   # or
   xmake run TestRHIApp
   # or
   xmake run CustomModule
   ```

3. **Check the console output** for:
   - Module registration messages
   - Module startup messages
   - No "not found in registry" errors
   - All expected modules listed in "Loaded Engine Modules"

4. **Verify module shutdown**:
   - Exit the application
   - Check for ShutdownModule() messages
   - Should shut down in reverse order

## Best Practices for Adding New Modules

When adding a new engine module, follow this pattern:

### 1. Create Module Directory Structure
```
src/Runtime/MyModule/
├── Public/
│   ├── MyModule.h          # Module interface
│   └── MyModuleAPI.h       # Public API
└── Private/
    ├── MyModule.cpp        # Module implementation + registration
    └── MyModuleImpl.cpp    # Implementation details
```

### 2. Create Module Interface (Public/MyModule.h)
```cpp
#pragma once
#include "Modules/ModuleInterface.h"

class FMyModule : public IModuleInterface
{
public:
    virtual ~FMyModule() override = default;
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual bool IsGameModule() const override { return false; }
};
```

### 3. Implement Module (Private/MyModule.cpp)
```cpp
#include "MyModule.h"
#include "Modules/Module.h"
#include "CoreUtils.h"

void FMyModule::StartupModule()
{
    LOG("MyModule: Startup - Initializing subsystem");
    // Initialize your subsystem here
}

void FMyModule::ShutdownModule()
{
    LOG("MyModule: Shutdown - Cleaning up subsystem");
    // Clean up resources here
}

// Register the module
IMPLEMENT_MODULE(FMyModule, MyModule)
```

### 4. Create xmake.lua
```lua
local kind = get_config("module_kind") or "shared"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end

set_basename("MyModule")

target("MyModule")
    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    add_deps("Core")  -- Add dependencies
    
    if kind == "shared" then
        add_defines("MYMODULE_BUILD_SHARED", {public = false})
        add_defines("MYMODULE_IMPORT_SHARED", {public = true})
    end
target_end()
```

### 5. Add to Root xmake.lua
```lua
includes("src/Runtime/MyModule")
```

### 6. Load in Launch.cpp
```cpp
FModuleManager::Get().LoadModule("MyModule");
```

## References

This implementation follows Unreal Engine's module system design:
- Module registration via macros
- Automatic registration before main()
- Explicit loading with dependency resolution
- Lifecycle hooks (Startup/Shutdown)
- Type-based categorization
- Ordered shutdown

For more information, see:
- `src/Runtime/Core/Public/Modules/Module.h` - Module manager and macros
- `src/Runtime/Core/Public/Modules/ModuleInterface.h` - Module interface
- `src/Runtime/Core/Public/Modules/ModuleDescriptor.h` - Module metadata
- `src/Runtime/Core/Private/Modules/ModuleExamples.cpp` - Example modules
