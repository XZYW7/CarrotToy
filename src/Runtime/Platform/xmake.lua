-- Platform module for CarrotToy

local kind = get_config("module_kind") or "shared"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end

set_basename("Platform")

target("Platform")
    -- Platform is a foundational module with minimal dependencies
    add_packages("glfw", {public = true})
    add_packages("glad", "imgui")
    
    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    
    -- Add defines for shared library build
    if kind == "shared" then
        add_defines("PLATFORM_BUILD_SHARED", {public = false})
        add_defines("PLATFORM_IMPORT_SHARED", {public = true})
    end
target_end()
