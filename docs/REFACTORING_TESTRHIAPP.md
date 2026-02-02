# TestApplicationModule 重构说明

## 问题

原先的 TestApplicationModule 被错误地放置在 Core 模块内部（`src/Runtime/Core/`），这违反了模块分离的设计原则。应用程序代码不应该混在引擎核心代码中。

## 解决方案

将 TestApplicationModule 重构为独立的应用程序 `TestRHIApp`，遵循与 `DefaultGame` 相同的结构模式。

## 重构详情

### 之前的结构（错误）：

```
src/Runtime/Core/
├── Public/Modules/
│   └── TestApplicationModule.h        ❌ 应用代码在核心模块中
└── Private/Modules/
    ├── TestApplicationModule.cpp       ❌ 应用代码在核心模块中
    └── TestApplicationModule_Register.cpp
```

### 现在的结构（正确）：

```
src/TestRHIApp/                         ✅ 独立的应用程序目录
├── Private/
│   ├── TestApplicationModule.h         ✅ 应用模块定义
│   ├── TestApplicationModule.cpp       ✅ 应用模块实现
│   └── TestRHIApp.cpp                  ✅ 应用入口点
└── xmake.lua                           ✅ 独立的构建配置
```

## 目录结构对比

### DefaultGame（参考示例）

```
src/DefaultGame/
├── Private/
│   ├── GameModules.h
│   ├── GameModules.cpp
│   └── DefualtGame.cpp         # 应用入口点
└── xmake.lua                   # 构建配置
```

### TestRHIApp（新结构）

```
src/TestRHIApp/
├── Private/
│   ├── TestApplicationModule.h
│   ├── TestApplicationModule.cpp
│   └── TestRHIApp.cpp          # 应用入口点
└── xmake.lua                   # 构建配置
```

## xmake.lua 配置

TestRHIApp 的 xmake.lua 配置与 DefaultGame 类似：

```lua
target("TestRHIApp")
    set_kind("binary")              -- 生成可执行文件
    add_files("Private/**.cpp")
    add_deps("Core", "Launch")      -- 依赖 Core 和 Launch 模块
    
    -- 系统库依赖
    if is_plat("windows") then
        add_syslinks("opengl32", "gdi32", "user32", "shell32")
        add_ldflags("/WHOLEARCHIVE:Launch.lib", {force = true})
    elseif is_plat("linux") then
        add_syslinks("GL", "pthread", "dl", "X11")
    elseif is_plat("macosx") then
        add_frameworks("OpenGL", "Cocoa", "IOKit", "CoreVideo")
    end
    
    set_targetdir("$(builddir)/bin")
    set_objectdir("$(builddir)/obj/TestRHIApp")
target_end()
```

## 根目录 xmake.lua 更新

在根目录的 xmake.lua 中添加了 TestRHIApp：

```lua
includes("src/Runtime/Launch")
includes("src/Runtime/Core")
includes("src/DefaultGame")
includes("src/TestRHIApp")        -- 新增
```

## 模块依赖关系

```
TestRHIApp (应用程序)
    ├─→ Core (引擎核心)
    │   └─→ 模块管理系统
    │       ├─→ ModuleInterface
    │       ├─→ ModuleManager
    │       └─→ EngineModules
    └─→ Launch (启动器)
        └─→ FMainLoop
```

## 使用方法

### 构建

```bash
xmake build TestRHIApp
```

### 运行

```bash
xmake run TestRHIApp
```

### 预期输出

```
launchDir /path/to/build/bin
projectDir /path/to/project
shaderWorkingDir /path/to/project/shaders
InternalProjectName TestRHIApplication
FMainLoop: Loading PreInit Modules
ModuleManager: Registering module CoreEngine of type 0
...
TestApplicationModule: Startup
TestApplicationModule: This is an example Application module for RHI testing
TestApplicationModule: Initializing RHI Test Environment
TestApplicationModule: RHI Test Environment initialized
TestApplicationModule: Running RHI Tests
TestApplicationModule: Test 1 - Basic RHI Initialization: PASS
TestApplicationModule: Test 2 - Render Target Creation: PASS
TestApplicationModule: Test 3 - Shader Compilation: PASS
TestApplicationModule: All RHI tests completed
```

## 代码变化

### TestRHIApp.cpp（新文件）

```cpp
#include "Launch.h"
#include "Modules/Module.h"
#include "TestApplicationModule.h"

// 注册 Test Application Module
IMPLEMENT_APPLICATION_MODULE(FTestApplicationModule, TestRHIApp, "TestRHIApplication")
```

这个文件类似于 DefaultGame 的入口点文件，使用 `IMPLEMENT_APPLICATION_MODULE` 宏来注册应用程序模块。

### 从 Core 模块移除的文件

- `src/Runtime/Core/Public/Modules/TestApplicationModule.h` ❌ 删除
- `src/Runtime/Core/Private/Modules/TestApplicationModule.cpp` ❌ 删除
- `src/Runtime/Core/Private/Modules/TestApplicationModule_Register.cpp` ❌ 删除

### ModuleExamples.cpp 更新

移除了直接访问 TestApplicationModule 的示例，改为说明它现在是独立应用程序：

```cpp
// 之前（错误）：
#include "Modules/TestApplicationModule.h"
FModuleManager::Get().LoadModule("TestApplication");
auto* testApp = static_cast<FTestApplicationModule*>(...);

// 现在（正确）：
LOG("TestRHIApp is now a separate application.");
LOG("To use it, build and run the TestRHIApp executable:");
LOG("  xmake build TestRHIApp");
LOG("  xmake run TestRHIApp");
```

## 设计原则

这次重构遵循了以下设计原则：

1. **关注点分离**：应用程序代码不应混在引擎核心代码中
2. **模块化**：每个应用程序都是独立的模块
3. **可重用性**：Core 模块可以被多个应用程序复用
4. **清晰的依赖关系**：通过 xmake.lua 明确定义依赖
5. **一致性**：TestRHIApp 和 DefaultGame 使用相同的结构模式

## 类似 UE 的项目结构

这种结构类似于 Unreal Engine 的项目组织方式：

```
UE 结构:
Engine/
  Source/
    Runtime/
      Core/               # 引擎核心
      Launch/             # 启动器
Games/
  MyGame/                 # 游戏项目
  MyTestApp/              # 测试应用

CarrotToy 结构:
src/
  Runtime/
    Core/                 # 引擎核心
    Launch/               # 启动器
  DefaultGame/            # 游戏项目
  TestRHIApp/             # 测试应用
```

## 优势

1. **清晰的模块边界**：Core 只包含引擎核心功能
2. **易于扩展**：可以轻松添加新的应用程序
3. **独立构建**：每个应用程序可以独立构建和运行
4. **更好的依赖管理**：通过 xmake 明确管理依赖关系
5. **符合最佳实践**：遵循 UE 的项目组织模式

## 总结

通过这次重构，TestApplicationModule 从 Core 模块中分离出来，成为一个独立的应用程序 TestRHIApp。这使得项目结构更加清晰，符合模块化设计原则，也更容易维护和扩展。

现在可以通过简单的命令来构建和运行 RHI 测试：

```bash
xmake build TestRHIApp
xmake run TestRHIApp
```

这种结构也为未来添加更多测试应用程序或其他类型的应用程序提供了良好的基础。
