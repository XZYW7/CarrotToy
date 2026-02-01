-- Core module for CarrotToy

local kind = get_config("module_kind") or "static"
if kind == "shared" then
    set_kind("shared")
else
    set_kind("static")
end

set_basename("Launch")

target("Launch")
    -- collect core sources; keep public headers in Public/ for consumers
    add_files("Private/**.cpp")
    add_headerfiles("Public/**.h")
    add_includedirs("Public", {public = true})
    add_deps("Core", "Editor")
target_end()