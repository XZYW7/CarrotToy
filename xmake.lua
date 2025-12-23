-- CarrotToy - Material Editor Lab
-- Xmake build configuration

set_project("CarrotToy")
set_version("0.1.0")
set_languages("c++17")

add_rules("mode.debug", "mode.release")

-- Add required packages
add_requires("glfw", "glad", "glm", "imgui", "stb")

-- Main application target
target("CarrotToy")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("include/*.h")
    add_includedirs("include")
    
    -- Link required packages
    add_packages("glfw", "glad", "glm", "imgui", "stb")
    
    -- Platform-specific settings
    if is_plat("windows") then
        add_syslinks("opengl32", "gdi32", "user32", "shell32")
    elseif is_plat("linux") then
        add_syslinks("GL", "pthread", "dl", "X11")
    elseif is_plat("macosx") then
        add_frameworks("OpenGL", "Cocoa", "IOKit", "CoreVideo")
    end
    
    -- Copy shaders to build directory
    after_build(function (target)
        os.cp("$(projectdir)/shaders", path.join(target:targetdir(), "shaders"))
    end)
    
    set_targetdir("build/bin")
    set_objectdir("build/obj")
target_end()
