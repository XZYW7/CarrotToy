# Module System Refactoring - Validation Checklist

## Build Validation

### Prerequisites
- Ensure xmake is installed
- Clean any previous build artifacts: `xmake clean -a`

### Build Steps

```bash
# 1. Configure the project
xmake f -m debug

# 2. Build all targets
xmake

# Expected: Should build successfully with no errors
# - Core module should build
# - Editor module should build (new)
# - Launch module should build
# - DefaultGame target should build
# - TestRHIApp target should build
```

## Runtime Validation

### Test DefaultGame Application

```bash
xmake run DefaultGame
```

**Expected Output:**
```
InternalProjectName CarrotToyProject
FMainLoop: Loading PreInit Modules
FMainLoop: Loading Application Module: DefaultGame
ModuleManager: Starting up module DefaultGame
DefaultGameModule: Startup
DefaultGameModule: This is the CarrotToy default game/editor application
ModuleManager: Module DefaultGame loaded successfully
ModuleManager: Starting up module CoreEngine
CoreEngineModule: Startup
CoreEngineModule: Initializing core engine systems
ModuleManager: Module CoreEngine loaded successfully
ModuleManager: Starting up module RHI
RHIModule: Startup
RHIModule: Initializing Render Hardware Interface
ModuleManager: Module RHI loaded successfully
ModuleManager: Starting up module Editor
EditorModule: Startup
EditorModule: Initializing editor systems
ModuleManager: Module Editor loaded successfully
FMainLoop: Loaded Application Modules:
  - DefaultGame
FMainLoop: Loaded Engine Modules:
  - CoreEngine
  - RHI
  - Editor
FMainLoop: Loaded Game Modules:
```

**Key Changes to Verify:**
- ✅ No "Module X not found in registry" errors for CoreEngine, RHI, or Editor
- ✅ No attempts to load DefaultGame or GameplayModule as Game modules
- ✅ Editor module loads successfully
- ✅ "Loaded Game Modules:" section is empty (GameModule concept removed)
- ✅ MaterialEditor functionality works (since it was moved to Editor module)

### Test TestRHIApp Application

```bash
xmake run TestRHIApp
```

**Expected Output:**
```
InternalProjectName TestRHIApp
FMainLoop: Loading PreInit Modules
FMainLoop: Loading Application Module: TestRHIApp
ModuleManager: Starting up module TestRHIApp
TestApplicationModule: Startup
TestApplicationModule: This is an example Application module for RHI testing
TestApplicationModule: Initializing RHI Test Environment
TestApplicationModule: RHI Test Environment initialized
ModuleManager: Module TestRHIApp loaded successfully
ModuleManager: Module CoreEngine loaded successfully
ModuleManager: Module RHI loaded successfully
ModuleManager: Module Editor loaded successfully
FMainLoop: Loaded Application Modules:
  - TestRHIApp
FMainLoop: Loaded Engine Modules:
  - CoreEngine
  - RHI
  - Editor
FMainLoop: Loaded Game Modules:
```

**Key Changes to Verify:**
- ✅ TestRHIApp loads as Application module
- ✅ IsGameModule() returns false for TestApplicationModule
- ✅ No Game modules are loaded

## Code Structure Validation

### Files That Should Exist
```
✅ src/Runtime/Editor/Public/EditorModule.h
✅ src/Runtime/Editor/Public/MaterialEditor.h
✅ src/Runtime/Editor/Private/EditorModule.cpp
✅ src/Runtime/Editor/Private/MaterialEditor.cpp
✅ src/Runtime/Editor/xmake.lua
✅ src/DefaultGame/Private/DefaultGameModule.h
✅ docs/MODULE_REFACTORING_2026_02_01.md
```

### Files That Should NOT Exist
```
❌ src/DefaultGame/Private/GameModules.h
❌ src/DefaultGame/Private/GameModules.cpp
❌ src/Runtime/Core/Public/MaterialEditor.h
❌ src/Runtime/Core/Private/MaterialEditor.cpp
```

### Code References to Check

```bash
# Should find NO references to FGameModule or FGameplayModule
grep -r "FGameModule\|FGameplayModule" src/ --include="*.cpp" --include="*.h" | grep -v "DefaultGameModule" | grep -v "IsGameModule"
# Expected: No results

# Should find Editor module includes in Launch
grep "EditorModule.h\|MaterialEditor.h" src/Runtime/Launch/Private/Launch.cpp
# Expected: Both includes present

# Should find Editor dependency in xmake files
grep "Editor" src/Runtime/Launch/xmake.lua src/DefaultGame/xmake.lua
# Expected: Both files have Editor in add_deps()
```

## Functionality Validation

### Material Editor Functionality
1. Launch DefaultGame
2. Verify MaterialEditor UI appears
3. Test material creation
4. Test material property editing
5. Test shader editing
6. Verify all MaterialEditor features work as before

**Expected:** MaterialEditor should work identically to before, since the code was just moved to a different module.

## Documentation Validation

### Updated Documentation
- ✅ `docs/MODULE_SYSTEM.md` - Updated with architecture notes and new directory structure
- ✅ `docs/MODULE_QUICK_REFERENCE.md` - Deprecated GameModule macros
- ✅ `docs/MODULE_REFACTORING_2026_02_01.md` - New refactoring summary document
- ✅ `TODO.md` - Marked Editor split task as complete

### Documentation Consistency
```bash
# Check that old GameModule references are noted as deprecated
grep -i "deprecated\|removed" docs/MODULE_*.md
# Expected: Multiple matches explaining deprecation
```

## Common Issues and Solutions

### Issue: Build fails with "EditorModule.h not found"
**Solution:** Ensure Editor module is included in root xmake.lua:
```lua
includes("src/Runtime/Editor")
```

### Issue: MaterialEditor symbols not found during linking
**Solution:** Ensure Launch and DefaultGame depend on Editor in their xmake.lua:
```lua
add_deps("Core", "Launch", "Editor")
```

### Issue: Editor module doesn't load at runtime
**Solution:** Check that Launch.cpp calls:
```cpp
FModuleManager::Get().LoadModule("Editor");
```

### Issue: "Module Editor not found in registry"
**Solution:** Verify EditorModule.cpp has:
```cpp
IMPLEMENT_MODULE(FEditorModule, Editor)
```

## Success Criteria

All of the following must be true:
- ✅ Project builds without errors
- ✅ DefaultGame runs and shows no module loading errors
- ✅ TestRHIApp runs and shows no module loading errors
- ✅ MaterialEditor UI works correctly
- ✅ No "Module X not found in registry" errors appear
- ✅ Only Application and Engine modules load (no Game modules)
- ✅ Documentation is consistent and up-to-date

## Next Steps

After validation passes:
1. Consider removing EModuleType::Game entirely from the codebase
2. Update any remaining old documentation files
3. Consider adding automated tests for module loading
4. Update tutorials and examples to use the new architecture
