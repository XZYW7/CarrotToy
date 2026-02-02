-- Renderer module for CarrotToy

local kind = get_config("module_kind") or "shared"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end

set_basename("Renderer")

target("Renderer")
    -- Renderer depends on Core and RHI
    add_deps("Core", "RHI")
    add_packages("glad", "glfw", {public = true})
    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    
    -- Add defines for shared library build
    if kind == "shared" then
        add_defines("RENDERER_BUILD_SHARED", {public = false})
        add_defines("RENDERER_IMPORT_SHARED", {public = true})
    end
target_end()
