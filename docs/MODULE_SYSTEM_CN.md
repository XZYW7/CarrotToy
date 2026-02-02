# 模块管理系统实现总结

## 概述

本次实现为 CarrotToy 项目添加了一套完整的模块管理机制，模仿了虚幻引擎（Unreal Engine）的模块系统架构。该系统提供了代码加载、插件管理、以及游戏和引擎项目分别管理的完整功能。

## 实现的功能

### 1. 模块类型系统 (EModuleType)

系统支持四种模块类型：

- **Engine（引擎模块）**: 核心引擎功能（如渲染、物理等）
- **Game（游戏模块）**: 游戏特定的逻辑和内容
- **Plugin（插件模块）**: 可选的可加载扩展
- **Application（应用程序模块）**: 应用程序入口点，用于特定任务

### 2. 模块管理器 (FModuleManager)

增强的模块管理器提供以下功能：

```cpp
// 注册模块
RegisterModule(name, module, type)

// 加载模块
LoadModule(name)

// 获取模块
GetModule(name)

// 检查模块是否已加载
IsModuleLoaded(name)

// 卸载模块
UnloadModule(name)

// 关闭所有模块
ShutdownAll()

// 插件管理
DiscoverPlugins(directory)
LoadPlugin(name)
UnloadPlugin(name)
GetAvailablePlugins()

// 按类型查询模块
GetModulesByType(type)
```

### 3. 模块描述符系统

#### FModuleDescriptor
包含模块元数据：
- 模块名称
- 模块类型
- 加载阶段
- 是否可卸载
- 依赖项列表

#### FPluginDescriptor
包含插件元数据：
- 插件名称
- 友好名称
- 版本
- 描述
- 作者
- 是否默认启用
- 包含的模块列表

### 4. 模块注册宏

提供了三个便捷的注册宏：

```cpp
// 注册引擎模块
IMPLEMENT_MODULE(FMyModule, MyModule)

// 注册游戏模块
IMPLEMENT_GAME_MODULE(FMyGameModule, MyGameModule)

// 注册应用程序模块
IMPLEMENT_APPLICATION_MODULE(FMyAppModule, MyApp, "MyAppName")
```

### 5. 示例实现

#### 引擎模块示例
- **FCoreEngineModule**: 核心引擎系统模块
- **FRHIModule**: 渲染硬件接口模块

#### 游戏模块示例
- **FGameModule**: 游戏特定系统模块
- **FGameplayModule**: 游戏玩法系统模块

#### 应用程序模块示例
- **TestRHIApp**: RHI测试独立应用程序
  - 包含 `FTestApplicationModule` 模块
  - `InitializeRHITest()`: 初始化RHI测试环境
  - `RunRHITests()`: 运行RHI测试套件
  - `ShutdownRHITest()`: 清理RHI测试环境
  - 作为独立应用程序运行，类似 DefaultGame

## 使用方法

### 创建一个新模块

1. 定义模块接口：

```cpp
// MyModule.h
#pragma once
#include "Modules/ModuleInterface.h"

class FMyModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
```

2. 实现模块：

```cpp
// MyModule.cpp
#include "MyModule.h"
#include "CoreUtils.h"

void FMyModule::StartupModule()
{
    LOG("MyModule: 启动");
    // 在此初始化你的系统
}

void FMyModule::ShutdownModule()
{
    LOG("MyModule: 关闭");
    // 在此清理你的系统
}

// 注册模块
#include "Modules/Module.h"
IMPLEMENT_MODULE(FMyModule, MyModule)
```

3. 加载模块：

```cpp
FModuleManager::Get().LoadModule("MyModule");
```

### 使用RHI测试应用程序

TestRHIApp 现在是一个独立的应用程序，不再作为模块加载：

```bash
# 构建 TestRHIApp
xmake build TestRHIApp

# 运行 TestRHIApp
xmake run TestRHIApp
```

TestRHIApp 会在启动时自动运行 RHI 测试。它的结构类似于 DefaultGame，是一个完全独立的应用程序。

## 模块生命周期

```
注册 → StartupModule() → 运行中 → ShutdownModule() → 卸载
```

### 关键生命周期方法

- `StartupModule()`: 模块加载后立即调用，初始化资源
- `ShutdownModule()`: 卸载前调用，清理资源
- `PreUnloadCallback()`: 卸载前调用（用于热重载场景）
- `PostLoadCallback()`: 重载后调用（用于热重载场景）

## 模块关闭顺序

系统确保模块按照正确的顺序关闭：

```
Application → Game → Plugin → Engine
```

这确保了依赖关系的正确处理。

## 目录结构

```
src/
├── Runtime/
│   ├── Core/                           # 核心引擎模块
│   │   ├── Public/
│   │   │   └── Modules/
│   │   │       ├── ModuleInterface.h       # 模块接口
│   │   │       ├── Module.h                # 模块管理器
│   │   │       ├── ModuleDescriptor.h      # 模块描述符
│   │   │       └── EngineModules.h         # 引擎模块示例
│   │   └── Private/
│   │       ├── Modules/
│   │       │   ├── Module.cpp              # 模块管理器实现
│   │       │   ├── EngineModules.cpp       # 引擎模块实现
│   │       │   └── ModuleExamples.cpp      # 使用示例
│   │       └── CoreModule.cpp              # 核心模块注册
│   └── Launch/                             # 应用启动器
│       └── Private/
│           └── Launch.cpp                  # 更新以使用新的模块系统
├── DefaultGame/                            # 游戏项目示例
│   ├── Private/
│   │   ├── GameModules.h                   # 游戏模块定义
│   │   ├── GameModules.cpp                 # 游戏模块实现
│   │   └── DefualtGame.cpp                 # 游戏入口点
│   └── xmake.lua                           # DefaultGame 构建配置
└── TestRHIApp/                             # RHI 测试应用程序
    ├── Private/
    │   ├── TestApplicationModule.h         # 测试应用模块定义
    │   ├── TestApplicationModule.cpp       # 测试应用模块实现
    │   └── TestRHIApp.cpp                  # 测试应用入口点
    └── xmake.lua                           # TestRHIApp 构建配置
```

## 文档

提供了两份详细的文档：

1. **MODULE_SYSTEM.md**: 完整的系统文档，包括：
   - 架构概述
   - 详细使用指南
   - 最佳实践
   - 故障排除
   - 未来增强计划

2. **MODULE_QUICK_REFERENCE.md**: 快速参考指南，包括：
   - 常用操作
   - 注册宏
   - 接口方法
   - 生命周期说明

## 主要特性

### 1. 依赖管理
模块可以声明依赖项，系统会自动按正确顺序加载：

```cpp
FModuleDescriptor desc;
desc.ModuleName = "MyModule";
desc.Dependencies.Add("CoreEngine");
desc.Dependencies.Add("RHI");
```

### 2. 插件发现
系统可以自动发现指定目录中的插件：

```cpp
FModuleManager::Get().DiscoverPlugins("/path/to/plugins");
```

### 3. 模块查询
可以按类型查询已加载的模块：

```cpp
auto engineModules = FModuleManager::Get().GetModulesByType(EModuleType::Engine);
auto gameModules = FModuleManager::Get().GetModulesByType(EModuleType::Game);
```

### 4. 日志记录
所有模块操作都有详细的日志记录，便于调试：

```
ModuleManager: Registering module CoreEngine of type 0
CoreEngineModule: Startup
CoreEngineModule: Initializing core engine systems
...
```

## RHI 测试应用程序详解

`TestRHIApp` 是一个完整的独立应用程序，专门用于RHI测试：

### 功能：
- RHI测试环境初始化
- 测试套件执行
- 自动资源清理
- 独立运行，不依赖其他应用程序

### 结构：
TestRHIApp 采用与 DefaultGame 相同的结构模式：
- 位于 `src/TestRHIApp/` 目录
- 有自己的 xmake.lua 构建配置
- 依赖 Core 和 Launch 模块
- 生成独立的可执行文件

### 使用场景：
- 完善RHI后的功能测试
- 渲染管线验证
- 性能基准测试
- 回归测试

### 运行方法：

```bash
# 构建
xmake build TestRHIApp

# 运行
xmake run TestRHIApp
```

### 扩展示例：

如需添加更多测试，修改 `TestApplicationModule.cpp` 中的 `RunRHITests()` 方法：

```cpp
void FTestApplicationModule::RunRHITests()
{
    if (!bRHITestInitialized) {
        LOG("Error: RHI Test Environment not initialized");
        return;
    }
    
    LOG("Running RHI Tests");
    
    // 测试1: 基本渲染管线
    TestBasicRenderingPipeline();
    
    // 测试2: 纹理创建和绑定
    TestTextureCreationAndBinding();
    
    // 测试3: 着色器编译和绑定
    TestShaderCompilationAndBinding();
    
    // 测试4: 渲染目标创建
    TestRenderTargetCreation();
    
    // 测试5: 缓冲区创建和更新
    TestBufferCreationAndUpdate();
    
    LOG("All RHI tests completed");
}
```

## 未来扩展

系统设计时考虑了未来的扩展性：

- [ ] 基于JSON的插件描述符（.uplugin文件）
- [ ] 热重载支持
- [ ] 模块版本控制和兼容性检查
- [ ] 异步模块加载
- [ ] 模块依赖图可视化
- [ ] 每个模块的配置文件

## 最佳实践

1. **明确依赖**: 显式声明模块依赖，确保正确的加载顺序
2. **关闭顺序**: 系统自动处理关闭顺序（应用→游戏→插件→引擎）
3. **无状态启动**: 不要在 `StartupModule()` 中依赖全局状态
4. **资源管理**: 总是在 `ShutdownModule()` 中清理资源
5. **正确的模块类型**: 为模块选择合适的类型

## 模块编译模式

CarrotToy 支持两种模块编译模式：

### 动态编译（默认）

默认情况下，所有模块都编译为**动态链接库（DLL/SO）**：

```bash
# 使用动态编译（默认）
xmake f --module_kind=shared
xmake
```

**优点：**
- 更快的增量构建速度
- 更小的最终可执行文件
- 运行时动态链接
- 更适合开发和快速迭代

### 静态编译

如果需要静态链接，可以配置为静态编译：

```bash
# 使用静态编译
xmake f --module_kind=static
xmake
```

**优点：**
- 单个可执行文件，包含所有代码
- 无需外部 DLL/SO 依赖
- 可执行文件较大
- 更适合最终发布

### 切换编译模式

要在两种模式之间切换，只需重新配置并构建：

```bash
# 从动态切换到静态
xmake f --module_kind=static
xmake clean
xmake

# 从静态切换到动态
xmake f --module_kind=shared
xmake clean
xmake
```

## 总结

本实现提供了一个完整、可扩展的模块管理系统，类似于虚幻引擎的架构。系统支持：

✅ **代码加载**: 静态和动态模块加载  
✅ **动态编译**: 默认使用动态链接库，支持切换到静态编译  
✅ **插件管理**: 插件发现和生命周期管理  
✅ **项目分离**: 引擎和游戏模块的独立管理  
✅ **应用程序模块**: 用于RHI测试的专用应用程序模块示例  
✅ **完整文档**: 详细的使用指南和快速参考  

该系统为CarrotToy项目提供了坚实的模块化基础，便于未来的扩展和维护。
