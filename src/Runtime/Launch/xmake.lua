-- Core module for CarrotToy

-- local kind = get_config("module_kind") or "shared"
-- if kind == "shared" then
--     set_kind("shared")
-- else
    set_kind("static")
-- end

set_basename("Launch")

target("Launch")
    -- collect core sources; keep public headers in Public/ for consumers
    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    add_deps("Renderer", "Editor")
    
    -- Add defines for shared library build
    if kind == "shared" then
        add_defines("LAUNCH_BUILD_SHARED", {public = false})
        add_defines("LAUNCH_IMPORT_SHARED", {public = true})
    end
target_end()