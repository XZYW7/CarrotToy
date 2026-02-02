# Module Linking and Registration Notes

## Context

This document explains how module registration works with different linking strategies (static vs dynamic).

## The Challenge

Module registration happens via global static constructors created by the `IMPLEMENT_MODULE` macro:

```cpp
IMPLEMENT_MODULE(FMyModule, MyModuleName)
```

This expands to a global `FStaticallyLinkedModuleRegistrant` object whose constructor registers the module with `FModuleManager`.

**The Problem**: Global constructors only run if:
1. The object file is linked into the final binary (for static libs)
2. The shared library is loaded at runtime (for dynamic libs)

## Static vs Dynamic Linking

### Dynamic Linking (Default: Core, RHI, Renderer, Editor)

**Build Configuration**:
```lua
local kind = get_config("module_kind") or "shared"
set_kind("shared")  -- Build as DLL/SO
```

**How Registration Works**:
1. Module is built as a shared library (.dll/.so/.dylib)
2. Application depends on the module via `add_deps("Core", "RHI", etc.)`
3. When application starts, the OS loader loads all dependent shared libraries
4. Global constructors in the shared library run automatically
5. Module is registered with `FModuleManager`

**Advantages**:
- No special linker flags needed
- Registration happens automatically
- Supports hot-reloading (future)
- Smaller executable size

**Requirements**:
- Module must be in the dependency chain (via `add_deps`)
- Shared library must be found at runtime (LD_LIBRARY_PATH, etc.)

### Static Linking (Launch Module Only)

**Build Configuration**:
```lua
set_kind("static")  -- Force static linking
```

**How Registration Works**:
1. Module is built as a static library (.lib/.a)
2. Linker includes object files from the static library in the final binary
3. **Problem**: Linker may exclude "unused" symbols (like module registration code)
4. **Solution**: Use whole-archive linking to force inclusion

**Linker Flags by Platform**:

**Windows**:
```lua
add_ldflags("/WHOLEARCHIVE:Launch.lib", {force = true})
```

**Linux**:
```lua
-- Note: xmake may handle this automatically for static dependencies
-- If needed, use:
-- add_ldflags("-Wl,--whole-archive", "-lLaunch", "-Wl,--no-whole-archive", {force = true})
```

**macOS**:
```lua
-- Note: xmake may handle this automatically for static dependencies
-- If needed, use:
-- add_ldflags("-Wl,-force_load,path/to/libLaunch.a", {force = true})
```

## Current Project Configuration

### Launch Module (Static)

Launch is **always** statically linked because:
1. It contains the entry point (`main()`)
2. Must be included in every application
3. Cannot be a shared library that might not load

Applications use:
- Windows: `/WHOLEARCHIVE:Launch.lib` to ensure all symbols are included
- Linux/macOS: xmake automatically handles static dependencies correctly

### Other Modules (Dynamic by Default)

Core, RHI, Renderer, Editor are **dynamically linked by default**:
- Built as shared libraries
- Loaded automatically via dependency chain
- No special linker flags needed
- Registration happens when the shared library is loaded

### Why This Works

The dependency chain ensures all modules are loaded:

```
DefaultGame
  ├─ Renderer (shared) ─────┐
  │   ├─ Core (shared)      │
  │   └─ RHI (shared) ──────┤
  │       └─ Core (shared)  │
  └─ Launch (static)        │
      └─ Renderer (shared) ─┘
```

When DefaultGame starts:
1. OS loader loads DefaultGame.exe
2. OS loader sees it depends on Renderer.dll, Launch (static, already in exe)
3. OS loader loads Renderer.dll → runs its global constructors → registers FRendererModule
4. Renderer.dll depends on Core.dll and RHI.dll
5. OS loader loads Core.dll → registers FCoreEngineModule
6. OS loader loads RHI.dll → registers FRHIModule
7. Launch is static, so its registration already ran
8. main() runs
9. FMainLoop::LoadPreInitModules() calls LoadModule() for each module
10. Modules are already registered, so LoadModule() succeeds and calls StartupModule()

## Building with Static Modules

If you want to build all modules as static libraries:

```bash
xmake f --module_kind=static
xmake
```

**Note**: With all-static builds, you'll need whole-archive flags for all module libraries on Linux/macOS. Windows already uses `/WHOLEARCHIVE:Launch.lib` which would need to be extended to include other modules.

## Troubleshooting

### "Module not found in registry" Error

**Symptom**: Module shows "not found in registry" when LoadModule() is called.

**Causes**:
1. Module's IMPLEMENT_MODULE never ran (registration didn't happen)
2. Module source file not compiled
3. Module library not linked
4. Static library needs whole-archive flag

**Solutions**:
1. Check that module .cpp file exists and has IMPLEMENT_MODULE
2. Verify xmake.lua includes the file: `add_files("Private/**.cpp")`
3. Verify application depends on the module: `add_deps("ModuleName")`
4. For static builds, ensure whole-archive flags are set

### Module Registration Happens but StartupModule Not Called

**Symptom**: Module appears in registry but startup code doesn't run.

**Cause**: LoadModule() was never called for that module.

**Solution**: Add LoadModule() call in Launch.cpp:
```cpp
FModuleManager::Get().LoadModule("ModuleName");
```

## Best Practices

1. **Use dynamic linking by default** for engine modules
   - Easier to develop and debug
   - No linker flag issues
   - Supports hot-reloading

2. **Use static linking only when necessary**
   - Entry point modules (Launch)
   - Platform-specific modules
   - Deployment optimization

3. **Always verify dependency chain**
   - Use `add_deps()` to establish dependencies
   - Transitive dependencies are automatically handled

4. **Test on all platforms**
   - Windows, Linux, and macOS handle shared libraries differently
   - Verify module registration on each platform

## References

- `src/Runtime/Core/Public/Modules/Module.h` - IMPLEMENT_MODULE macro definition
- `src/Runtime/Core/Private/Modules/Module.cpp` - FModuleManager implementation
- `docs/MODULE_SYSTEM_REFACTORING_2026_02_02.md` - Complete module system documentation
