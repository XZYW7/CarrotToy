-- Input module for CarrotToy

local kind = get_config("module_kind") or "shared"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end

set_basename("Input")

target("Input")
    add_deps("Core", "Platform")
    
    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    -- Explicitly add dependency include paths to fix C1083 in some build environments
    add_includedirs("../Platform/Public")
    add_includedirs("../Core/Public")
    
    -- Add defines for shared library build
    if kind == "shared" then
        add_defines("INPUT_BUILD_SHARED", {public = false})
        add_defines("INPUT_IMPORT_SHARED", {public = true})
    end
target_end()
