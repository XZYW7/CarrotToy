# Proper DLL Module Registration Solution

## Date: 2026-02-02

## Problem with Previous Approach

The previous solution used dummy "Force*ModuleLoad()" functions to force DLL loading. This approach had several issues:

### Issues:
1. ❌ **Not a proper solution** - Dummy functions are a workaround, not a real fix
2. ❌ **Linking errors** - "LINK : fatal error LNK1181: 无法打开输入文件"Core.lib""
3. ❌ **Build issues** - Cannot switch between debug and release modes
4. ❌ **Architectural problem** - Module symbols should be properly exported, not use dummy functions

## Root Cause

On Windows with DLLs, the linker only loads shared libraries if:
1. Symbols are explicitly imported from the DLL
2. The import library (.lib) is correctly generated and linked

The previous approach relied on global constructors in DLLs, which has problems:
- Global constructors run when DLL loads
- But DLL only loads if symbols are referenced
- Dummy functions were a hack to create references
- This doesn't solve the underlying export/import issue

## Proper Solution

### Exported Initialization Functions

Instead of relying on global constructors or dummy functions, we now use **explicit, exported initialization functions**:

```cpp
// In each dynamic module (e.g., RHIModule.cpp)
IMPLEMENT_MODULE_WITH_API(FRHIModule, RHI, RHI_API)

// This expands to:
extern "C" RHI_API void InitializeModuleRHI()
{
    static bool bInitialized = false;
    if (!bInitialized) {
        FModuleManager::Get().RegisterModule(TEXT("RHI"),
            FUniquePtr<IModuleInterface>(new FRHIModule()),
            EModuleType::Engine);
        bInitialized = true;
    }
}
```

### Why This Works

1. **Explicit Export**: Function is marked with `RHI_API` (`__declspec(dllexport)`)
2. **Import Library Generated**: Linker creates .lib with symbol references
3. **Proper Linking**: Applications import the function and can call it
4. **No Global Constructors Needed**: Registration happens when init function is called
5. **Cleaner**: No dummy functions, proper DLL architecture

## Implementation Details

### Module Macro Variants

**For Dynamic/Shared Modules:**
```cpp
// With explicit export macro
IMPLEMENT_MODULE_WITH_API(FMyModule, MyModule, MY_MODULE_API)

// With default CORE_API (backward compatibility)
IMPLEMENT_MODULE(FMyModule, MyModule)
```

**For Static Modules:**
```cpp
// Uses global constructor (automatic registration)
IMPLEMENT_MODULE_STATIC(FLaunchModule, Launch)
```

**For Application Modules:**
```cpp
// In executable, uses static registration
IMPLEMENT_APPLICATION_MODULE(FMyAppModule, MyApp, "MyApp")
```

### Export Macros

Each module defines its export macro:

**Core** (CoreUtils.h):
```cpp
#ifdef CORE_BUILD_SHARED
    #define CORE_API __declspec(dllexport)
#elif defined(CORE_IMPORT_SHARED)
    #define CORE_API __declspec(dllimport)
#else
    #define CORE_API
#endif
```

**RHI** (RHITypes.h):
```cpp
#ifdef RHI_BUILD_SHARED
    #define RHI_API __declspec(dllexport)
#elif defined(RHI_IMPORT_SHARED)
    #define RHI_API __declspec(dllimport)
#else
    #define RHI_API
#endif
```

**Renderer** (RendererAPI.h):
```cpp
#ifdef RENDERER_BUILD_SHARED
    #define RENDERER_API __declspec(dllexport)
#elif defined(RENDERER_IMPORT_SHARED)
    #define RENDERER_API __declspec(dllimport)
#else
    #define RENDERER_API
#endif
```

### Build Configuration

xmake.lua sets these defines:

**When building the DLL:**
```lua
add_defines("RHI_BUILD_SHARED", {public = false})
```

**When using the DLL:**
```lua
add_defines("RHI_IMPORT_SHARED", {public = true})
```

## Usage

### In Module Implementation

**Core Module** (CoreModule.cpp):
```cpp
#include "Modules/EngineModules.h"
// ... implementation ...

// Uses default CORE_API
IMPLEMENT_MODULE(FCoreEngineModule, CoreEngine)
```

**RHI Module** (RHIModule.cpp):
```cpp
#include "RHI/RHITypes.h"
// ... implementation ...

// Explicitly uses RHI_API
IMPLEMENT_MODULE_WITH_API(FRHIModule, RHI, RHI_API)
```

**Renderer Module** (RendererModule.cpp):
```cpp
#include "RendererAPI.h"
// ... implementation ...

// Explicitly uses RENDERER_API
IMPLEMENT_MODULE_WITH_API(FRendererModule, Renderer, RENDERER_API)
```

**Launch Module** (LaunchModule.cpp):
```cpp
// Static module, uses global constructor
IMPLEMENT_MODULE_STATIC(FLaunchModule, Launch)
```

### In Application Code

**Launch.cpp LoadPreInitModules():**
```cpp
void FMainLoop::LoadPreInitModules()
{
    LOG("FMainLoop: Loading PreInit Modules");
    
    // Initialize dynamic modules by calling exported init functions
    InitializeModuleCoreEngine();  // Calls Core.dll's init function
    InitializeModuleRHI();          // Calls RHI.dll's init function
    InitializeModuleRenderer();     // Calls Renderer.dll's init function
    
    // Now load the modules (they're registered!)
    FModuleManager::Get().LoadModule("CoreEngine");
    FModuleManager::Get().LoadModule("Launch");
    FModuleManager::Get().LoadModule("RHI");
    FModuleManager::Get().LoadModule("Renderer");
}
```

### Module Headers

Each module provides a header declaring its init function:

**EngineModules.h:**
```cpp
extern "C" CORE_API void InitializeModuleCoreEngine();
```

**RHIModuleInit.h:**
```cpp
extern "C" RHI_API void InitializeModuleRHI();
```

**RendererModule.h:**
```cpp
extern "C" RENDERER_API void InitializeModuleRenderer();
```

## Comparison: Before vs After

### Before (Dummy Functions)

```cpp
// Dummy function approach (WRONG)
extern "C" void ForceCoreModuleLoad()
{
    // Empty function just to force DLL loading
}

// In Launch.cpp
ForceCoreModuleLoad();  // Just forces DLL to load
// Hope global constructors registered the module...
```

**Problems:**
- ❌ Dummy function has no real purpose
- ❌ Doesn't guarantee proper export/import
- ❌ Linking errors
- ❌ Not explicit about what happens

### After (Proper Exports)

```cpp
// Proper exported init function (CORRECT)
extern "C" RHI_API void InitializeModuleRHI()
{
    static bool bInitialized = false;
    if (!bInitialized) {
        FModuleManager::Get().RegisterModule(...);
        bInitialized = true;
    }
}

// In Launch.cpp
InitializeModuleRHI();  // Explicitly initializes and registers the module
```

**Benefits:**
- ✅ Properly exported symbol
- ✅ Explicit initialization
- ✅ Clear what happens when called
- ✅ Follows Windows DLL best practices
- ✅ Similar to Unreal Engine's approach

## Module Lifecycle

```
Program Start
    ↓
Static Modules (Launch, Application):
  → Global constructors run
  → IMPLEMENT_MODULE_STATIC creates registrant
  → Module registered automatically ✅
    ↓
Dynamic Modules (Core, RHI, Renderer):
  → DLLs loaded (via add_deps in xmake)
  → But modules not registered yet
    ↓
LoadPreInitModules():
  → Calls InitializeModuleCoreEngine()
  → Calls InitializeModuleRHI()
  → Calls InitializeModuleRenderer()
  → Each function registers its module ✅
    ↓
  → FModuleManager::LoadModule("CoreEngine")
  → Finds module in registry ✅
  → Calls StartupModule() ✅
    ↓
Modules Active and Running
    ↓
Shutdown:
  → FModuleManager::ShutdownAll()
  → Calls ShutdownModule() on each
  → Modules cleaned up ✅
```

## Benefits of This Approach

### 1. Proper DLL Architecture
- ✅ Uses standard Windows DLL export/import
- ✅ Generates proper import libraries (.lib)
- ✅ No workarounds or hacks

### 2. Explicit and Clear
- ✅ Clear what each init function does
- ✅ Easy to understand module initialization
- ✅ Explicit control over registration timing

### 3. Similar to Unreal Engine
- ✅ UE also uses exported init functions
- ✅ Industry-standard pattern
- ✅ Well-understood approach

### 4. Solves All Issues
- ✅ No linking errors (proper .lib generation)
- ✅ Works in debug and release
- ✅ No dummy functions
- ✅ Proper symbol export/import

### 5. Maintainable
- ✅ Easy to add new modules
- ✅ Clear pattern to follow
- ✅ Well-documented

## Adding New Modules

To add a new dynamic module:

### 1. Create Export Macro (in module's public header)
```cpp
// MyModule/Public/MyModuleAPI.h
#ifndef MY_MODULE_API
#if defined(_WIN32) || defined(_WIN64)
    #ifdef MY_MODULE_BUILD_SHARED
        #define MY_MODULE_API __declspec(dllexport)
    #elif defined(MY_MODULE_IMPORT_SHARED)
        #define MY_MODULE_API __declspec(dllimport)
    #else
        #define MY_MODULE_API
    #endif
#else
    #define MY_MODULE_API
#endif
#endif
```

### 2. Implement Module
```cpp
// MyModule/Private/MyModule.cpp
#include "MyModuleAPI.h"
#include "Modules/Module.h"

class FMyModule : public IModuleInterface
{
    // ... implementation ...
};

// Register with proper export
IMPLEMENT_MODULE_WITH_API(FMyModule, MyModule, MY_MODULE_API)
```

### 3. Declare Init Function
```cpp
// MyModule/Public/MyModuleInit.h
#include "MyModuleAPI.h"

extern "C" MY_MODULE_API void InitializeModuleMyModule();
```

### 4. Configure Build
```lua
-- MyModule/xmake.lua
if kind == "shared" then
    add_defines("MY_MODULE_BUILD_SHARED", {public = false})
    add_defines("MY_MODULE_IMPORT_SHARED", {public = true})
end
```

### 5. Initialize in Application
```cpp
// Launch.cpp
#include "MyModuleInit.h"

void FMainLoop::LoadPreInitModules()
{
    // ...
    InitializeModuleMyModule();
    // ...
    FModuleManager::Get().LoadModule("MyModule");
}
```

## Conclusion

This solution properly handles Windows DLL module registration by:
- Using explicit exported initialization functions
- Proper __declspec(dllexport/dllimport) declarations
- Following industry-standard patterns
- Avoiding workarounds and hacks
- Maintaining clean, understandable code

The linking errors and build issues should now be resolved. ✅
