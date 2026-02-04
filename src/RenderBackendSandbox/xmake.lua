local kind = get_config("module_kind") or "shared"

set_basename("RenderBackendSandbox")

target("RenderBackendSandbox")
    set_kind("binary")
    
    add_files("Private/**.cpp")
    add_headerfiles("Private/**.h")
    add_includedirs("Private", {public = false})
    
    add_deps("Core", "Platform", "RHI", "Launch")

    -- Application packages
    add_packages("glfw", "directxshadercompiler", "glm")

    -- System libraries and link options
    if is_plat("windows") then
        add_syslinks("opengl32", "gdi32", "user32", "shell32", "d3d12", "dxgi", "d3dcompiler")
        add_ldflags("/WHOLEARCHIVE:Launch.lib", {force = true})
    elseif is_plat("linux") then
        add_syslinks("GL", "pthread", "dl", "X11", "vulkan")
    elseif is_plat("macosx") then
        add_frameworks("OpenGL", "Cocoa", "IOKit", "CoreVideo", "Metal", "QuartzCore")
    end

    -- Custom build rules (compile shaders)
    add_rules("utils.compile_shaders")

    set_targetdir("$(builddir)/bin")
    set_objectdir("$(builddir)/obj/RenderBackendSandbox")
target_end()
