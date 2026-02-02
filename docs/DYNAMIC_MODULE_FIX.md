# Dynamic Module Registration Fix

## Date: 2026-02-02

## Problem

When running applications (TestRHIApp, DefaultGame, CustomModule) with dynamically linked modules, only statically linked modules were being registered with the ModuleManager. The console output showed:

```
ModuleManager: Registering module TestRHIApp of type 3
ModuleManager: Registering module Launch of type 0
...
ModuleManager: Module CoreEngine not found in registry
ModuleManager: Module RHI not found in registry
ModuleManager: Module Renderer not found in registry
```

### Why This Happened

The module system uses the `IMPLEMENT_MODULE` macro to register modules:

```cpp
IMPLEMENT_MODULE(FMyModule, MyModuleName)
```

This macro expands to create a **global static object**:

```cpp
static FStaticallyLinkedModuleRegistrant<FMyModule> ModuleRegistrantMyModuleName(TEXT("MyModuleName"), EModuleType::Engine);
```

The constructor of this global object registers the module with `FModuleManager`.

**For static libraries:**
- The linker includes the object file in the final binary
- The global constructor runs before `main()`
- Module is registered ✅

**For dynamic libraries (DLLs/SOs):**
- The global constructor only runs when the DLL is loaded
- On Windows, DLLs are only loaded if symbols are explicitly referenced
- Without references, the linker doesn't load the DLL
- Global constructors never run
- Module is never registered ❌

### The Chicken-and-Egg Problem

1. To call `LoadModule("CoreEngine")`, the module must be registered
2. For Core DLL's registration code to run, the DLL must be loaded
3. For the DLL to be loaded, symbols must be referenced
4. But we can't reference symbols until we know the module is loaded!

## Solution

### Add Force-Load Functions

Created explicit functions in each module that applications can reference to force DLL loading:

**Core Module** (`EngineModules.h/cpp`):
```cpp
extern "C" void ForceCoreModuleLoad()
{
    // This function exists solely to be referenced from applications
    // to ensure the Core DLL is loaded, triggering global constructors
    // that register the module with FModuleManager
}
```

**RHI Module** (`RHIModule.cpp`):
```cpp
extern "C" void ForceRHIModuleLoad()
{
    // Forces RHI DLL to load
}
```

**Renderer Module** (`RendererModule.cpp`):
```cpp
extern "C" void ForceRendererModuleLoad()
{
    // Forces Renderer DLL to load
}
```

### Call Force-Load Functions Before Loading Modules

In `Launch.cpp`, added calls before attempting to load modules:

```cpp
void FMainLoop::LoadPreInitModules()
{
    LOG("FMainLoop: Loading PreInit Modules");
    
    // Force dynamic module DLLs to load by referencing symbols from them
    // This ensures their global constructors run and modules get registered
    ForceCoreModuleLoad();
    ForceRHIModuleLoad();
    ForceRendererModuleLoad();
    
    // Now load the modules (they're registered!)
    FModuleManager::Get().LoadModule("CoreEngine");
    FModuleManager::Get().LoadModule("RHI");
    FModuleManager::Get().LoadModule("Renderer");
    // ...
}
```

### How It Works

1. **Symbol Reference Created**: Application calls `ForceCoreModuleLoad()`
2. **Linker Requirement**: Linker must satisfy the reference to this function
3. **DLL Loaded**: Core.dll is loaded to provide the function
4. **Constructors Run**: All global constructors in Core.dll execute
5. **Module Registered**: `IMPLEMENT_MODULE(FCoreEngineModule, CoreEngine)` runs, registering the module
6. **LoadModule Succeeds**: `FModuleManager::Get().LoadModule("CoreEngine")` finds the module in registry
7. **StartupModule Called**: Module's `StartupModule()` is invoked

## Why `extern "C"`?

Functions are declared with `extern "C"` to:
- Prevent C++ name mangling
- Make the symbol name predictable across compilers
- Ensure the linker can find the symbol

Without `extern "C"`, the function name would be mangled (e.g., `_Z19ForceCoreModuleLoadv`), making it harder to reference reliably.

## Alternative Solutions (Not Used)

### 1. Make Everything Static
**Pros**: Simple, always works
**Cons**: 
- Larger executable size
- No hot-reloading capability
- User wants dynamic linking

### 2. Use `/WHOLEARCHIVE` for All Modules
**Pros**: Forces all symbols to be included
**Cons**:
- Only works for static libraries
- User wants dynamic linking
- Defeats the purpose of DLLs

### 3. Explicit DLL Loading with LoadLibrary
**Pros**: Direct control over DLL loading
**Cons**:
- Platform-specific code
- Need to find DLL paths at runtime
- More complex

### 4. Module Init Functions (Chosen Solution)
**Pros**:
- Clean, simple API
- Works with dynamic linking
- Platform-independent
- Minimal code changes
**Cons**:
- Need to remember to call force-load functions
- Slightly verbose

## Expected Output After Fix

```
ModuleManager: Registering module TestRHIApp of type 3
ModuleManager: Registering module CoreEngine of type 0    ← NEW!
ModuleManager: Registering module Launch of type 0
ModuleManager: Registering module RHI of type 0           ← NEW!
ModuleManager: Registering module Renderer of type 0       ← NEW!

FMainLoop: Loading PreInit Modules
FMainLoop: Loading Application Module: TestRHIApp
ModuleManager: Starting up module TestRHIApp
TestApplicationModule: Startup
...

ModuleManager: Starting up module CoreEngine               ← NEW!
CoreEngineModule: Startup                                  ← NEW!
CoreEngineModule: Initializing core engine systems         ← NEW!
ModuleManager: Module CoreEngine loaded successfully       ← NEW!

ModuleManager: Starting up module Launch
LaunchModule: Startup - Initializing launch subsystem
ModuleManager: Module Launch loaded successfully

ModuleManager: Starting up module RHI                      ← NEW!
RHIModule: Startup - Initializing RHI subsystem            ← NEW!
ModuleManager: Module RHI loaded successfully              ← NEW!

ModuleManager: Starting up module Renderer                 ← NEW!
RendererModule: Startup - Initializing renderer subsystem  ← NEW!
ModuleManager: Module Renderer loaded successfully         ← NEW!

FMainLoop: Loaded Engine Modules:
  - CoreEngine                                             ← NEW!
  - Launch
  - RHI                                                    ← NEW!
  - Renderer                                               ← NEW!
```

## Files Changed

### Modified Files:
1. `src/Runtime/Core/Public/Modules/EngineModules.h`
   - Added `ForceCoreModuleLoad()` declaration

2. `src/Runtime/Core/Private/Modules/EngineModules.cpp`
   - Added `ForceCoreModuleLoad()` implementation

3. `src/Runtime/RHI/Private/RHIModule.cpp`
   - Added `ForceRHIModuleLoad()` function

4. `src/Runtime/Renderer/Public/RendererModule.h`
   - Added `ForceRendererModuleLoad()` declaration

5. `src/Runtime/Renderer/Private/RendererModule.cpp`
   - Added `ForceRendererModuleLoad()` implementation

6. `src/Runtime/Launch/Private/Launch.cpp`
   - Added includes for module headers
   - Call force-load functions in `LoadPreInitModules()`

### New Files:
7. `src/Runtime/RHI/Public/RHI/RHIModule.h`
   - Created with `ForceRHIModuleLoad()` declaration

## Benefits

✅ **Keeps Dynamic Linking**: Modules remain as DLLs/SOs as user requested  
✅ **Cross-Platform**: Works on Windows, Linux, macOS  
✅ **Clean Architecture**: Minimal changes to existing code  
✅ **Future-Proof**: Pattern can be reused for new modules  
✅ **Hot-Reload Ready**: Dynamic modules can still be reloaded (future feature)

## Usage for New Modules

When creating a new dynamically linked module:

1. **Add force-load function** in module implementation:
```cpp
extern "C" void ForceMyModuleLoad() {}
```

2. **Declare it** in public header:
```cpp
extern "C" void ForceMyModuleLoad();
```

3. **Call it** in Launch.cpp before LoadModule:
```cpp
ForceMyModuleLoad();
FModuleManager::Get().LoadModule("MyModule");
```

## References

- [MODULE_LINKING_NOTES.md](./MODULE_LINKING_NOTES.md) - Technical linking details
- [MODULE_SYSTEM_REFACTORING_2026_02_02.md](./MODULE_SYSTEM_REFACTORING_2026_02_02.md) - Module system overview
- `src/Runtime/Core/Public/Modules/Module.h` - IMPLEMENT_MODULE macro definition
