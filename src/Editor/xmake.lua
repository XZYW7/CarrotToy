-- Editor module for CarrotToy

local kind = get_config("module_kind") or "shared"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end

set_basename("Editor")

target("Editor")
    -- Editor now uses Platform's ImGui abstraction
    -- Only need imgui for ImGui:: API calls, not the backends
    add_packages("imgui")

    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    add_deps("Core", "Renderer")
    
    -- Add defines for shared library build
    if kind == "shared" then
        add_defines("EDITOR_BUILD_SHARED", {public = false})
        add_defines("EDITOR_IMPORT_SHARED", {public = true})
    end
target_end()
