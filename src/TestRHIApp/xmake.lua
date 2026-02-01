local kind = get_config("module_kind") or "static"

set_basename("TestRHIApp")

target("TestRHIApp")
    set_kind("binary")
    
    add_files("Private/**.cpp")
    add_deps("Core", "Launch")

    -- 应用依赖包
    add_packages("directxshadercompiler")

    -- 应用系统库
    if is_plat("windows") then
        add_syslinks("opengl32", "gdi32", "user32", "shell32")
        -- 强制链接 Launch 的 main
        add_ldflags("/WHOLEARCHIVE:Launch.lib", {force = true})
    elseif is_plat("linux") then
        add_syslinks("GL", "pthread", "dl", "X11")
    elseif is_plat("macosx") then
        add_frameworks("OpenGL", "Cocoa", "IOKit", "CoreVideo")
    end

    -- 应用自定义构建规则 (编译 Shader)
    add_rules("utils.compile_shaders")

    set_targetdir("$(builddir)/bin")
    set_objectdir("$(builddir)/obj/TestRHIApp")
target_end()
