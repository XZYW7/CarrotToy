-- RHI module for CarrotToy

local kind = get_config("module_kind") or "shared"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end

set_basename("RHI")

target("RHI")
    -- collect RHI sources; keep public headers in Public/ for consumers
    add_packages("glad", "glfw", "glm", {public = true})

    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    
    -- RHI depends on Core for basic types and utilities
    add_deps("Core")
    
    -- Add defines for shared library build
    if kind == "shared" then
        add_defines("RHI_BUILD_SHARED", {public = false})
        add_defines("RHI_IMPORT_SHARED", {public = true})
    end
target_end()
