# Migration Guide: New Module Architecture

This guide helps you migrate code from the old module structure to the new, improved architecture.

## What Changed?

### New Modules
1. **Platform Module**: Extracted from Core, now a separate foundational module
2. **Input Module**: New dedicated module for input handling

### Updated Dependencies
- **Core**: Now depends on Platform (instead of containing it)
- **RHI**: No longer depends on GLFW (cleaner abstraction)
- **Renderer**: Now depends on Platform and Input explicitly
- **All Modules**: Cleaner dependency chain

## Code Migration

### 1. Platform Access

**Before**:
```cpp
// Platform was part of Core
#include "CoreUtils.h"  // Had platform included implicitly

// Platform types were in Core
using namespace CarrotToy::Platform;
```

**After**:
```cpp
// Platform is now a separate module
#include "Platform/Platform.h"
#include "Platform/PlatformTypes.h"

// Platform namespace is still the same
using namespace CarrotToy::Platform;
```

### 2. Input Handling

**Before (Direct Window Access)**:
```cpp
// Input was accessed through Renderer or Window
double x, y;
renderer->getCursorPos(x, y);
bool pressed = renderer->getMouseButton(0);

// Or through window directly
window->getCursorPos(x, y);
bool pressed = window->getMouseButton(0);
```

**After (Input Module)**:
```cpp
#include "Input/InputDevice.h"

// Get input device from renderer
auto inputDevice = renderer->getInputDevice();

// Use proper input API
auto pos = inputDevice->getCursorPosition();
bool pressed = inputDevice->isMouseButtonPressed(Input::MouseButton::Left);

// Or create input device directly from window
auto inputDevice = Input::createInputDevice(window);
```

**Note**: The old Renderer input proxy methods still exist for backward compatibility but are deprecated.

### 3. Module Dependencies in xmake.lua

**Before**:
```lua
target("MyModule")
    add_deps("Core", "RHI")
    add_packages("glfw", {public = true})
```

**After**:
```lua
target("MyModule")
    add_deps("Core", "Platform", "Input", "RHI")
    -- GLFW is only needed if directly using windowing
    -- Most modules won't need it anymore
```

### 4. RHI Module

**Before**:
```cpp
// RHI had GLFW dependencies
#include "RHI/RHI.h"
#include <GLFW/glfw3.h>  // This was in OpenGLRHI.cpp
```

**After**:
```cpp
// RHI is now pure graphics abstraction
#include "RHI/RHI.h"
// No GLFW dependency needed
```

### 5. Include Paths

**Before**:
```cpp
// Platform was in Core
#include "Platform/Platform.h"  // Resolved from Core module
```

**After**:
```cpp
// Platform is its own module
#include "Platform/Platform.h"  // Resolved from Platform module
```

## Module Structure Changes

### Core Module
**Removed**:
- `src/Runtime/Core/Public/Platform/` (moved to Platform module)
- `src/Runtime/Core/Private/Platform/` (moved to Platform module)

**Kept**:
- Core utilities (CoreUtils.h)
- Material and Shader abstractions
- Scene objects
- Module system

### New Platform Module
**Location**: `src/Runtime/Platform/`

**Contains**:
- Platform abstraction (IPlatform, IPlatformWindow)
- Platform types (PlatformTypes.h)
- GLFW implementation

### New Input Module
**Location**: `src/Runtime/Input/`

**Contains**:
- Input device abstraction (IInputDevice)
- Input types (MouseButton, KeyCode, etc.)
- Platform-based implementation

## Build System Changes

### Building Modules

**Before**:
```bash
xmake build Core    # Had Platform inside
xmake build RHI     # Had GLFW dependency
```

**After**:
```bash
xmake build Platform  # Build platform first
xmake build Input     # Build input (depends on Platform)
xmake build Core      # Build core (depends on Platform)
xmake build RHI       # Build RHI (depends on Core only)
```

### Dependency Order

The new dependency order is:
1. **Platform** (foundational, only GLFW dependency)
2. **Core** (depends on Platform)
3. **Input** (depends on Platform)
4. **RHI** (depends on Core)
5. **Renderer** (depends on Core, Platform, Input, RHI)
6. **Launch** (depends on all above)
7. **Applications** (depend on Launch, Editor, etc.)

## Common Issues and Solutions

### Issue 1: Cannot Find Platform Headers

**Error**:
```
fatal error: 'Platform/Platform.h' file not found
```

**Solution**:
Add Platform module dependency:
```lua
target("YourTarget")
    add_deps("Platform")
```

### Issue 2: Input Methods Not Found

**Error**:
```
error: 'class CarrotToy::Renderer' has no member named 'getInputDevice'
```

**Solution**:
Update to the new Renderer.h which includes Input module:
```cpp
#include "Renderer.h"
auto inputDevice = renderer->getInputDevice();
```

### Issue 3: RHI Build Errors with GLFW

**Error**:
```
error: 'glfwGetProcAddress' was not declared in this scope
```

**Solution**:
The RHI module should not use GLFW directly. Remove GLFW includes from RHI code. Use the proc address loader passed during initialization instead.

### Issue 4: Circular Dependencies

**Error**:
```
error: circular dependency detected
```

**Solution**:
Follow the proper dependency hierarchy:
- Low-level: Platform → Core → RHI
- Mid-level: Input (uses Platform)
- High-level: Renderer (uses all above)

## Testing Your Migration

### Step 1: Update Dependencies
1. Update your module's xmake.lua
2. Add Platform and Input dependencies if needed
3. Remove unnecessary GLFW dependencies

### Step 2: Update Includes
1. Change Platform includes to use Platform module
2. Add Input includes where needed
3. Update RHI includes to remove GLFW

### Step 3: Update Code
1. Replace direct window input access with Input module
2. Update platform code to use new Platform module
3. Remove GLFW code from RHI if present

### Step 4: Build and Test
```bash
xmake f -c      # Clean configuration
xmake           # Build all
xmake run YourApp  # Test your application
```

## Benefits of Migration

After migrating to the new architecture, you'll get:

1. **Cleaner Dependencies**: Each module depends only on what it needs
2. **Better Testability**: Modules can be tested in isolation
3. **Easier Maintenance**: Clear boundaries make changes safer
4. **Platform Portability**: Platform abstraction is cleaner
5. **Input Flexibility**: Input can be extended independently
6. **RHI Purity**: RHI is now truly graphics-API-only

## Need Help?

- See [MODULE_ARCHITECTURE.md](MODULE_ARCHITECTURE.md) for detailed architecture docs
- Check the updated README.md for build instructions
- Look at DefaultGame, TestRHIApp for example usage
- Check existing modules for reference implementations

## Checklist

Use this checklist to ensure complete migration:

- [ ] Updated module dependencies in xmake.lua
- [ ] Added Platform module dependency where needed
- [ ] Added Input module dependency where needed
- [ ] Updated Platform includes to use Platform module
- [ ] Updated input code to use Input module
- [ ] Removed unnecessary GLFW dependencies
- [ ] Removed GLFW code from RHI module
- [ ] Build succeeds without errors
- [ ] Application runs correctly
- [ ] Input works as expected
- [ ] Rendering works as expected
