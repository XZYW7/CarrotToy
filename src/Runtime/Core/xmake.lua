-- Core module for CarrotToy

local kind = get_config("module_kind") or "static"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end


set_basename("Core")
target("Core")
    -- collect core sources; keep public headers in Public/ for consumers
    add_packages("glad","glfw","stb","imgui", "glm", {public = true})

    add_files("Private/**.cpp")
    -- Exclude RHI files as they are now in a separate module
    remove_files("Private/RHI/**.cpp")
    add_headerfiles("Public/**.h")
    -- Exclude RHI headers as they are now in a separate module
    remove_headerfiles("Public/RHI/**.h")
    add_includedirs("Public", {public = true})
target_end()