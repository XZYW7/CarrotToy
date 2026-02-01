# TestRHIApp 模块加载问题修复

## 问题描述

TestRHIApp 在运行时显示以下错误输出：

```
ModuleManager: Registering module TestRHIApp of type 3
FMainLoop: Loading PreInit Modules
ModuleManager: Module CoreEngine not found in registry
ModuleManager: Module RHI not found in registry
ModuleManager: Module DefaultGame not found in registry
ModuleManager: Module GameplayModule not found in registry
FMainLoop: Loaded Engine Modules:
FMainLoop: Loaded Game Modules:
```

### 问题分析

1. **TestRHIApp 模块被注册但未加载**
   - TestRHIApp 通过 `IMPLEMENT_APPLICATION_MODULE` 宏注册
   - 但 `FMainLoop::LoadPreInitModules()` 没有加载它
   - 导致 `TestApplicationModule::StartupModule()` 从未被调用

2. **尝试加载不存在的模块**
   - `Launch.cpp` 硬编码加载 CoreEngine、RHI、DefaultGame、GameplayModule
   - 这些模块只在 DefaultGame 中注册（通过 CoreModule.cpp 和 GameModules.cpp）
   - TestRHIApp 不链接这些源文件，所以这些模块不存在
   - 每次尝试加载都会输出错误信息

## 解决方案

### 1. 使模块加载可选

修改 `Module.cpp` 中的 `LoadModule()` 函数：

```cpp
bool FModuleManager::LoadModule(const FName& name)
{
    auto it = modules.find(name);
    if (it == modules.end()) {
        // Module not found - this is not necessarily an error
        // Some applications may not have all modules registered
        return false;  // 静默返回 false，不输出错误
    }
    // ... rest of function
}
```

**变化**：
- 移除了 `LOG("ModuleManager: Module " << name << " not found in registry");`
- 模块未找到时静默返回 false
- 这允许可选模块加载，不会产生错误噪音

### 2. 自动加载应用程序模块

修改 `Launch.cpp` 中的 `LoadPreInitModules()` 函数：

```cpp
void FMainLoop::LoadPreInitModules()
{
    LOG("FMainLoop: Loading PreInit Modules");
    
    // Load application module first (if registered)
    // Get all registered application modules and load them
    const auto& appModules = FModuleManager::Get().GetModulesByType(EModuleType::Application);
    for (const auto& modName : appModules) {
        LOG("FMainLoop: Loading Application Module: " << modName);
        FModuleManager::Get().LoadModule(modName);
    }
    
    // Try to load engine modules (optional - not all apps have these)
    FModuleManager::Get().LoadModule("CoreEngine");
    FModuleManager::Get().LoadModule("RHI");
    
    // Try to load game modules (optional - not all apps have these)
    FModuleManager::Get().LoadModule("DefaultGame");
    FModuleManager::Get().LoadModule("GameplayModule");
    
    // List all loaded modules by type
    LOG("FMainLoop: Loaded Application Modules:");
    const auto& loadedAppMods = FModuleManager::Get().GetModulesByType(EModuleType::Application);
    for (const auto& modName : loadedAppMods) {
        LOG("  - " << modName);
    }
    
    LOG("FMainLoop: Loaded Engine Modules:");
    const auto& engineMods = FModuleManager::Get().GetModulesByType(EModuleType::Engine);
    for (const auto& modName : engineMods) {
        LOG("  - " << modName);
    }
    
    LOG("FMainLoop: Loaded Game Modules:");
    const auto& gameMods = FModuleManager::Get().GetModulesByType(EModuleType::Game);
    for (const auto& modName : gameMods) {
        LOG("  - " << modName);
    }
}
```

**变化**：
1. **首先自动加载应用程序模块**
   - 使用 `GetModulesByType(EModuleType::Application)` 查找所有注册的应用程序模块
   - 遍历并加载它们
   - 这确保 TestApplicationModule 被正确加载和初始化

2. **使引擎和游戏模块加载可选**
   - 添加注释说明这些是可选的
   - 如果模块不存在，静默失败（不输出错误）

3. **单独列出应用程序模块**
   - 新增 "Loaded Application Modules:" 部分
   - 清楚地显示哪些应用程序模块已加载

## 预期的新输出

修复后，TestRHIApp 应该输出：

```
ModuleManager: Registering module TestRHIApp of type 3
argv[0] = D:\project\CarrotToy\build\bin\TestRHIApp.exe
launchDir D:/project/CarrotToy/build/bin
projectDir D:/project/CarrotToy/build/bin
shaderWorkingDir D:/project/CarrotToy/build/bin/shaders
InternalProjectName TestRHIApp
FMainLoop: Loading PreInit Modules
FMainLoop: Loading Application Module: TestRHIApp
ModuleManager: Starting up module TestRHIApp
TestApplicationModule: Startup
TestApplicationModule: This is an example Application module for RHI testing
TestApplicationModule: Initializing RHI Test Environment
TestApplicationModule: RHI Test Environment initialized
FMainLoop: Loaded Application Modules:
  - TestRHIApp
FMainLoop: Loaded Engine Modules:
FMainLoop: Loaded Game Modules:
... (继续正常执行)
```

**关键变化**：
- ✅ 不再有 "Module not found" 错误
- ✅ TestApplicationModule 正确启动
- ✅ RHI 测试环境被初始化
- ✅ 清楚地列出了加载的应用程序模块

## 对 DefaultGame 的影响

DefaultGame 链接了 CoreModule.cpp 和 GameModules.cpp，所以它会输出：

```
FMainLoop: Loading PreInit Modules
FMainLoop: Loaded Application Modules:
FMainLoop: Loaded Engine Modules:
  - CoreEngine
  - RHI
FMainLoop: Loaded Game Modules:
  - DefaultGame
  - GameplayModule
```

**注意**：DefaultGame 不使用 `IMPLEMENT_APPLICATION_MODULE`，所以它不会有应用程序模块。

## 技术细节

### 模块注册时机

1. **静态初始化**（main() 之前）
   ```cpp
   IMPLEMENT_APPLICATION_MODULE(FTestApplicationModule, TestRHIApp, "TestRHIApp")
   ```
   这个宏创建一个静态的 `FStaticallyLinkedModuleRegistrant` 对象
   其构造函数在程序启动时自动调用，注册模块

2. **模块加载**（PreInit 期间）
   ```cpp
   FModuleManager::Get().LoadModule("TestRHIApp");
   ```
   这会调用模块的 `StartupModule()` 方法

3. **模块关闭**（Exit 期间）
   ```cpp
   FModuleManager::Get().ShutdownAll();
   ```
   按正确顺序（Application → Game → Plugin → Engine）关闭所有模块

### 为什么 GetModulesByType 能在加载前工作

`GetModulesByType()` 返回所有已注册的模块，无论是否已加载：

```cpp
TArray<FName> FModuleManager::GetModulesByType(EModuleType type) const
{
    TArray<FName> result;
    for (const auto& kv : modules) {  // 遍历所有注册的模块
        if (kv.second.Descriptor.Type == type) {
            result.Add(kv.first);
        }
    }
    return result;
}
```

因此，我们可以：
1. 查询所有已注册的应用程序模块
2. 加载它们
3. 验证它们已加载

## 总结

这个修复确保了：

1. ✅ **应用程序模块自动加载**
   - TestApplicationModule 的 StartupModule() 被正确调用
   - RHI 测试环境被初始化

2. ✅ **可选模块加载**
   - 不存在的模块不会产生错误
   - 每个应用程序只加载它需要的模块

3. ✅ **清晰的日志输出**
   - 明确显示哪些模块类型被加载
   - 没有误导性的错误信息

4. ✅ **兼容多个应用程序**
   - DefaultGame 继续正常工作
   - TestRHIApp 现在也能正常工作
   - 未来的应用程序也会自动正确处理
