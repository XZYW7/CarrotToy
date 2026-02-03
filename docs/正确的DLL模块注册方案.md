# 正确的DLL模块注册方案

## 日期: 2026-02-02

## 之前方案的问题

之前的解决方案使用了虚拟的 "Force*ModuleLoad()" 函数来强制加载DLL。这种方法存在几个问题：

### 问题:
1. ❌ **不是正确的解决方案** - 虚拟函数是权宜之计，不是真正的修复
2. ❌ **链接错误** - "LINK : fatal error LNK1181: 无法打开输入文件"Core.lib""
3. ❌ **构建问题** - 无法在debug和release模式之间切换
4. ❌ **架构问题** - 模块符号应该被正确导出，而不是使用虚拟函数

## 根本原因

在Windows中使用DLL时，链接器只在以下情况下加载共享库：
1. 符号被显式地从DLL导入
2. 导入库(.lib)被正确生成和链接

之前的方法依赖于DLL中的全局构造函数，这存在问题：
- 全局构造函数在DLL加载时运行
- 但DLL只在符号被引用时才加载
- 虚拟函数是创建引用的hack方法
- 这没有解决底层的导出/导入问题

## 正确的解决方案

### 导出的初始化函数

不再依赖全局构造函数或虚拟函数，我们现在使用**显式的、导出的初始化函数**：

```cpp
// 在每个动态模块中（例如，RHIModule.cpp）
IMPLEMENT_MODULE_WITH_API(FRHIModule, RHI, RHI_API)

// 这会展开为：
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

### 为什么这样有效

1. **显式导出**: 函数被标记为 `RHI_API` (`__declspec(dllexport)`)
2. **生成导入库**: 链接器创建带有符号引用的.lib文件
3. **正确链接**: 应用程序导入函数并可以调用它
4. **不需要全局构造函数**: 注册发生在调用init函数时
5. **更清晰**: 没有虚拟函数，正确的DLL架构

## 实现细节

### 模块宏变体

**对于动态/共享模块:**
```cpp
// 使用显式的导出宏
IMPLEMENT_MODULE_WITH_API(FMyModule, MyModule, MY_MODULE_API)

// 使用默认的CORE_API（向后兼容）
IMPLEMENT_MODULE(FMyModule, MyModule)
```

**对于静态模块:**
```cpp
// 使用全局构造函数（自动注册）
IMPLEMENT_MODULE_STATIC(FLaunchModule, Launch)
```

**对于应用程序模块:**
```cpp
// 在可执行文件中，使用静态注册
IMPLEMENT_APPLICATION_MODULE(FMyAppModule, MyApp, "MyApp")
```

### 导出宏

每个模块定义其导出宏：

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

## 使用方法

### 在模块实现中

**Core模块** (CoreModule.cpp):
```cpp
#include "Modules/EngineModules.h"
// ... 实现 ...

// 使用默认的CORE_API
IMPLEMENT_MODULE(FCoreEngineModule, CoreEngine)
```

**RHI模块** (RHIModule.cpp):
```cpp
#include "RHI/RHITypes.h"
// ... 实现 ...

// 显式使用RHI_API
IMPLEMENT_MODULE_WITH_API(FRHIModule, RHI, RHI_API)
```

**Renderer模块** (RendererModule.cpp):
```cpp
#include "RendererAPI.h"
// ... 实现 ...

// 显式使用RENDERER_API
IMPLEMENT_MODULE_WITH_API(FRendererModule, Renderer, RENDERER_API)
```

**Launch模块** (LaunchModule.cpp):
```cpp
// 静态模块，使用全局构造函数
IMPLEMENT_MODULE_STATIC(FLaunchModule, Launch)
```

### 在应用程序代码中

**Launch.cpp LoadPreInitModules():**
```cpp
void FMainLoop::LoadPreInitModules()
{
    LOG("FMainLoop: Loading PreInit Modules");
    
    // 通过调用导出的init函数初始化动态模块
    InitializeModuleCoreEngine();  // 调用Core.dll的init函数
    InitializeModuleRHI();          // 调用RHI.dll的init函数
    InitializeModuleRenderer();     // 调用Renderer.dll的init函数
    
    // 现在加载模块（它们已经注册了！）
    FModuleManager::Get().LoadModule("CoreEngine");
    FModuleManager::Get().LoadModule("Launch");
    FModuleManager::Get().LoadModule("RHI");
    FModuleManager::Get().LoadModule("Renderer");
}
```

## 对比：之前 vs 之后

### 之前（虚拟函数）

```cpp
// 虚拟函数方法（错误）
extern "C" void ForceCoreModuleLoad()
{
    // 空函数只是为了强制DLL加载
}

// 在Launch.cpp中
ForceCoreModuleLoad();  // 只是强制加载DLL
// 希望全局构造函数已经注册了模块...
```

**问题:**
- ❌ 虚拟函数没有真正的用途
- ❌ 不保证正确的导出/导入
- ❌ 链接错误
- ❌ 不明确发生了什么

### 之后（正确的导出）

```cpp
// 正确的导出init函数（正确）
extern "C" RHI_API void InitializeModuleRHI()
{
    static bool bInitialized = false;
    if (!bInitialized) {
        FModuleManager::Get().RegisterModule(...);
        bInitialized = true;
    }
}

// 在Launch.cpp中
InitializeModuleRHI();  // 显式初始化并注册模块
```

**优点:**
- ✅ 正确导出的符号
- ✅ 显式初始化
- ✅ 清楚调用时发生什么
- ✅ 遵循Windows DLL最佳实践
- ✅ 类似于Unreal Engine的方法

## 模块生命周期

```
程序启动
    ↓
静态模块（Launch、Application）:
  → 全局构造函数运行
  → IMPLEMENT_MODULE_STATIC创建registrant
  → 模块自动注册 ✅
    ↓
动态模块（Core、RHI、Renderer）:
  → DLL加载（通过xmake中的add_deps）
  → 但模块还未注册
    ↓
LoadPreInitModules():
  → 调用InitializeModuleCoreEngine()
  → 调用InitializeModuleRHI()
  → 调用InitializeModuleRenderer()
  → 每个函数注册其模块 ✅
    ↓
  → FModuleManager::LoadModule("CoreEngine")
  → 在注册表中找到模块 ✅
  → 调用StartupModule() ✅
    ↓
模块激活并运行
    ↓
关闭:
  → FModuleManager::ShutdownAll()
  → 对每个模块调用ShutdownModule()
  → 模块清理完成 ✅
```

## 这种方法的优点

### 1. 正确的DLL架构
- ✅ 使用标准的Windows DLL导出/导入
- ✅ 生成正确的导入库(.lib)
- ✅ 没有权宜之计或hack

### 2. 显式和清晰
- ✅ 清楚每个init函数的作用
- ✅ 易于理解模块初始化
- ✅ 显式控制注册时机

### 3. 类似Unreal Engine
- ✅ UE也使用导出的init函数
- ✅ 行业标准模式
- ✅ 广为人知的方法

### 4. 解决所有问题
- ✅ 没有链接错误（正确的.lib生成）
- ✅ 在debug和release中都有效
- ✅ 没有虚拟函数
- ✅ 正确的符号导出/导入

### 5. 可维护
- ✅ 易于添加新模块
- ✅ 清晰的模式可循
- ✅ 文档齐全

## 添加新模块

要添加新的动态模块：

### 1. 创建导出宏（在模块的公共头文件中）
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

### 2. 实现模块
```cpp
// MyModule/Private/MyModule.cpp
#include "MyModuleAPI.h"
#include "Modules/Module.h"

class FMyModule : public IModuleInterface
{
    // ... 实现 ...
};

// 使用正确的导出注册
IMPLEMENT_MODULE_WITH_API(FMyModule, MyModule, MY_MODULE_API)
```

### 3. 声明Init函数
```cpp
// MyModule/Public/MyModuleInit.h
#include "MyModuleAPI.h"

extern "C" MY_MODULE_API void InitializeModuleMyModule();
```

### 4. 配置构建
```lua
-- MyModule/xmake.lua
if kind == "shared" then
    add_defines("MY_MODULE_BUILD_SHARED", {public = false})
    add_defines("MY_MODULE_IMPORT_SHARED", {public = true})
end
```

### 5. 在应用程序中初始化
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

## 结论

这个解决方案通过以下方式正确处理Windows DLL模块注册：
- 使用显式的导出初始化函数
- 正确的__declspec(dllexport/dllimport)声明
- 遵循行业标准模式
- 避免权宜之计和hack
- 保持代码清晰、易懂

链接错误和构建问题现在应该已经解决。✅
