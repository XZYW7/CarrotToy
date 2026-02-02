# Dynamic Module Loading - Visual Explanation

## The Problem: DLL Loading Chain

### Before Fix - Broken Chain ❌

```
┌─────────────────────────────────────────────────────────────┐
│ TestRHIApp.exe Startup                                      │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Static Code from Launch.lib                                 │
│  • main() function                                          │
│  • LaunchModule registration (runs before main)            │
│  • IMPLEMENT_MODULE(FLaunchModule, Launch) ✅               │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Linker checks: Do we need Core.dll?                        │
│  • Scan TestRHIApp code for Core symbols                   │
│  • No direct references found                              │
│  • Decision: DON'T LOAD CORE.DLL ❌                         │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
                 (Core.dll not loaded)
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Core.dll global constructors NEVER RUN ❌                   │
│  • IMPLEMENT_MODULE(FCoreEngineModule, CoreEngine)          │
│  • Never registers with FModuleManager                      │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ FMainLoop::LoadPreInitModules()                            │
│  • LoadModule("CoreEngine")                                │
│  • FModuleManager checks registry                          │
│  • Result: "Module CoreEngine not found in registry" ❌     │
└─────────────────────────────────────────────────────────────┘
```

### After Fix - Complete Chain ✅

```
┌─────────────────────────────────────────────────────────────┐
│ TestRHIApp.exe Startup                                      │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Static Code from Launch.lib                                 │
│  • main() function                                          │
│  • LaunchModule registration (runs before main)            │
│  • IMPLEMENT_MODULE(FLaunchModule, Launch) ✅               │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ FMainLoop::LoadPreInitModules()                            │
│  • ForceCoreModuleLoad()    ← NEW! ✅                       │
│  • ForceRHIModuleLoad()     ← NEW! ✅                       │
│  • ForceRendererModuleLoad() ← NEW! ✅                      │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Linker: Resolve ForceCoreModuleLoad()                      │
│  • Symbol is referenced by Launch code                     │
│  • Must load Core.dll to satisfy reference                 │
│  • Decision: LOAD CORE.DLL ✅                               │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Core.dll Loads                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ Global Constructor: ModuleRegistrantCoreEngine       │  │
│  │  • Runs automatically when DLL loads                 │  │
│  │  • IMPLEMENT_MODULE(FCoreEngineModule, CoreEngine)   │  │
│  │  • Calls FModuleManager::RegisterModule()           │  │
│  │  • Module is now in registry! ✅                     │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ Function: ForceCoreModuleLoad()                      │  │
│  │  • Returns immediately (empty function)              │  │
│  │  • Job is done: DLL loaded, constructor ran ✅       │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                        │
                        ▼
┌─────────────────────────────────────────────────────────────┐
│ Continue LoadPreInitModules()                               │
│  • LoadModule("CoreEngine")                                │
│  • FModuleManager finds it in registry ✅                   │
│  • Calls FCoreEngineModule::StartupModule() ✅              │
│  • Result: "Module CoreEngine loaded successfully" ✅       │
└─────────────────────────────────────────────────────────────┘
```

## Module Registration Timeline

### Before Fix

```
Time →
─────────────────────────────────────────────────────────────────
Before main()
│
├─ TestRHIApp global constructors run
│  └─ IMPLEMENT_APPLICATION_MODULE registers TestRHIApp ✅
│
├─ Launch.lib global constructors run (static)
│  └─ IMPLEMENT_MODULE registers Launch ✅
│
├─ Core.dll NOT LOADED ❌
├─ RHI.dll NOT LOADED ❌
└─ Renderer.dll NOT LOADED ❌

─────────────────────────────────────────────────────────────────
main()
│
└─ GEngineLoop.PreInit()
   │
   └─ FMainLoop::LoadPreInitModules()
      │
      ├─ LoadModule("CoreEngine") → NOT FOUND ❌
      ├─ LoadModule("RHI") → NOT FOUND ❌
      └─ LoadModule("Renderer") → NOT FOUND ❌
```

### After Fix

```
Time →
─────────────────────────────────────────────────────────────────
Before main()
│
├─ TestRHIApp global constructors run
│  └─ IMPLEMENT_APPLICATION_MODULE registers TestRHIApp ✅
│
└─ Launch.lib global constructors run (static)
   └─ IMPLEMENT_MODULE registers Launch ✅

─────────────────────────────────────────────────────────────────
main()
│
└─ GEngineLoop.PreInit()
   │
   └─ FMainLoop::LoadPreInitModules()
      │
      ├─ ForceCoreModuleLoad()         ← Loads Core.dll
      │  └─ Core.dll loads
      │     └─ IMPLEMENT_MODULE registers CoreEngine ✅
      │
      ├─ ForceRHIModuleLoad()          ← Loads RHI.dll
      │  └─ RHI.dll loads
      │     └─ IMPLEMENT_MODULE registers RHI ✅
      │
      ├─ ForceRendererModuleLoad()     ← Loads Renderer.dll
      │  └─ Renderer.dll loads
      │     └─ IMPLEMENT_MODULE registers Renderer ✅
      │
      ├─ LoadModule("CoreEngine") → FOUND ✅ → StartupModule()
      ├─ LoadModule("RHI") → FOUND ✅ → StartupModule()
      └─ LoadModule("Renderer") → FOUND ✅ → StartupModule()
```

## Dependency Loading

### How Dependencies Work

```
TestRHIApp.exe
│
├─ Depends on Launch (static) → Always included
│  └─ Launch depends on Renderer (shared)
│     └─ Renderer depends on Core, RHI (shared)
│        ├─ Core (shared)
│        └─ RHI (shared) → depends on Core
│
└─ With Force-Load Functions:
   │
   ├─ ForceCoreModuleLoad() called
   │  └─ Core.dll LOADS ✅
   │     └─ Global constructors run
   │        └─ CoreEngine registers
   │
   ├─ ForceRHIModuleLoad() called
   │  └─ RHI.dll LOADS ✅
   │     └─ Global constructors run
   │        └─ RHI registers
   │
   └─ ForceRendererModuleLoad() called
      └─ Renderer.dll LOADS ✅
         └─ Global constructors run
            └─ Renderer registers
```

## Symbol Reference Chain

### Why extern "C" Matters

```
C++ Function Declaration (without extern "C"):
─────────────────────────────────────────────────
void ForceCoreModuleLoad();

Compiled Symbol Name (mangled):
→ _Z19ForceCoreModuleLoadv
→ Compiler-specific
→ Hard to predict


C Function Declaration (with extern "C"):
─────────────────────────────────────────────────
extern "C" void ForceCoreModuleLoad();

Compiled Symbol Name (not mangled):
→ ForceCoreModuleLoad
→ Same across compilers ✅
→ Easy to reference ✅
```

## Module State Transitions

```
╔═══════════════════════════════════════════════════════════╗
║ Module Lifecycle with Force-Load                         ║
╚═══════════════════════════════════════════════════════════╝

┌───────────┐
│ NOT       │  DLL file exists but not loaded
│ LOADED    │
└─────┬─────┘
      │ ForceModuleLoad() called
      │ → Creates symbol reference
      │ → Linker loads DLL
      ▼
┌───────────┐
│ LOADED    │  DLL in memory
│           │  Global constructors run
└─────┬─────┘
      │ IMPLEMENT_MODULE executes
      │ → Registers with FModuleManager
      ▼
┌───────────┐
│ REGISTERED│  In FModuleManager registry
│           │  Ready to be loaded
└─────┬─────┘
      │ LoadModule() called
      │ → Found in registry ✅
      │ → Calls StartupModule()
      ▼
┌───────────┐
│ ACTIVE    │  Module running
│           │  Providing services
└─────┬─────┘
      │ ShutdownAll() called
      │ → Calls ShutdownModule()
      ▼
┌───────────┐
│ SHUTDOWN  │  Module cleaned up
│           │
└───────────┘
```

## Code Flow Comparison

### Before Fix - Failure Path

```
┌──────────────────────────────────────────────────────────┐
│ 1. Program starts                                        │
│    TestRHIApp.exe                                        │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│ 2. Static initialization                                 │
│    • Launch module registers (static lib) ✅             │
│    • TestRHIApp module registers ✅                      │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│ 3. main() → PreInit() → LoadPreInitModules()            │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│ 4. Try to load CoreEngine                               │
│    LoadModule("CoreEngine")                             │
│    └─> FModuleManager::modules.find("CoreEngine")      │
│        └─> NOT FOUND ❌                                  │
└──────────────────────────────────────────────────────────┘
                     │
                     ▼
              [FAILURE] ❌
```

### After Fix - Success Path

```
┌──────────────────────────────────────────────────────────┐
│ 1. Program starts                                        │
│    TestRHIApp.exe                                        │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│ 2. Static initialization                                 │
│    • Launch module registers (static lib) ✅             │
│    • TestRHIApp module registers ✅                      │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│ 3. main() → PreInit() → LoadPreInitModules()            │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│ 4. Force module loading                                  │
│    ForceCoreModuleLoad()                                │
│    └─> Linker loads Core.dll                           │
│        └─> Global constructors run                      │
│            └─> CoreEngine registers ✅                   │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ▼
┌──────────────────────────────────────────────────────────┐
│ 5. Load CoreEngine                                       │
│    LoadModule("CoreEngine")                             │
│    └─> FModuleManager::modules.find("CoreEngine")      │
│        └─> FOUND ✅                                      │
│            └─> StartupModule() ✅                        │
└────────────────────┬─────────────────────────────────────┘
                     │
                     ▼
              [SUCCESS] ✅
```

## Summary

The fix works by:
1. **Creating explicit symbol references** via Force*ModuleLoad() functions
2. **Forcing DLL loading** to satisfy those references
3. **Triggering global constructors** when DLL loads
4. **Registering modules** via IMPLEMENT_MODULE in constructors
5. **Enabling LoadModule()** to find registered modules

Result: All dynamically linked modules properly register and load! ✅
