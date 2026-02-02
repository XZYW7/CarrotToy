# 模块动态编译转换完成

## 问题描述
将引擎中的模块从静态编译改为动态编译。

## 实现方案

### 1. 修改默认编译模式
- 将根目录 `xmake.lua` 中的 `module_kind` 选项默认值从 `"static"` 改为 `"shared"`
- 所有模块（Core, RHI, Renderer, Editor, Launch）现在默认编译为动态链接库（DLL/SO）

### 2. 添加导出/导入宏
为每个模块添加了正确的 DLL 导出/导入宏：

#### Core 模块
- 更新了 `CoreUtils.h` 中的 `CORE_API` 宏
- 在编译模块时定义 `CORE_BUILD_SHARED` 并导出符号
- 在使用模块时定义 `CORE_IMPORT_SHARED` 并导入符号

#### RHI 模块  
- 在 `RHITypes.h` 中添加了 `RHI_API` 宏
- 配置了相应的导出/导入定义

#### Renderer、Launch、Editor 模块
- 为每个模块创建了专用的 API 头文件：
  - `RendererAPI.h` - 定义 `RENDERER_API`
  - `LaunchAPI.h` - 定义 `LAUNCH_API`
  - `EditorAPI.h` - 定义 `EDITOR_API`

### 3. 更新构建配置
- 所有模块的 `xmake.lua` 文件现在默认使用 `"shared"` 模式
- 在 Windows 平台上，`/WHOLEARCHIVE` 链接器标志现在仅在静态编译时使用
- 应用程序二进制文件（DefaultGame、TestRHIApp、CustomModule）会自动链接动态库

### 4. 向后兼容性
用户仍然可以通过以下命令使用静态编译：
```bash
xmake f --module_kind=static
xmake
```

## 技术细节

### Windows 平台
- 使用 `__declspec(dllexport)` 导出符号
- 使用 `__declspec(dllimport)` 导入符号
- 每个模块在编译时定义自己的 `*_BUILD_SHARED` 标志
- 依赖模块接收 `*_IMPORT_SHARED` 标志

### 非 Windows 平台
- API 宏为空，使用默认符号可见性
- 动态库使用 `.so` 扩展名（Linux）或 `.dylib`（macOS）

## 优势

### 动态编译（默认）
✅ 更快的增量构建速度  
✅ 更小的可执行文件  
✅ 运行时动态链接  
✅ 更适合开发和快速迭代  

### 静态编译（可选）
✅ 单个可执行文件  
✅ 无外部依赖  
✅ 更适合最终发布  

## 文档更新
- 更新了 `README.md` 的构建选项部分
- 在 `docs/BUILD.md` 中添加了"模块编译模式"章节
- 在 `docs/MODULE_SYSTEM_CN.md` 中添加了详细的编译模式说明

## 测试建议
1. 测试默认构建（动态编译）：
   ```bash
   xmake
   xmake run DefaultGame
   ```

2. 测试静态编译：
   ```bash
   xmake f --module_kind=static
   xmake clean
   xmake
   xmake run DefaultGame
   ```

3. 测试所有应用程序：
   - DefaultGame
   - TestRHIApp  
   - CustomModule

## 注意事项
- 动态编译模式下，运行应用程序时需要确保 DLL/SO 文件在系统路径中
- xmake 会自动将动态库复制到可执行文件所在目录
- 如果遇到"找不到 DLL"错误，请检查构建输出目录

## 完成状态
✅ 所有代码修改已完成  
✅ API 导出宏已添加  
✅ 文档已更新  
✅ 代码审查已通过  
✅ 安全检查已通过  
