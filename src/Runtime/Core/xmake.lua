-- Core module for CarrotToy

local kind = get_config("module_kind") or "shared"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end


set_basename("Core")
target("Core")
    -- Core is the foundational module and should not depend on other runtime modules
    -- collect core sources; keep public headers in Public/ for consumers
    add_packages("glm", {public = true})
    add_packages("stb")
    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    
    -- Add defines for shared library build
    if kind == "shared" then
        add_defines("CORE_BUILD_SHARED", {public = false})
        add_defines("CORE_IMPORT_SHARED", {public = true})
    end
    
target_end()