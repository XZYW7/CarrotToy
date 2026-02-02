-- CarrotToy - Material Editor Lab
-- Xmake build configuration

set_project("CarrotToy")
set_version("0.1.0")
set_languages("c++17")
if is_plat("windows") then
    add_cxflags("/utf-8")
end
add_rules("mode.debug", "mode.release")

add_requires("glfw", "glm", "stb", "directxshadercompiler")
add_requires("glad", {configs = {version = "4.6", extensions = "GL_ARB_gl_spirv"}})
add_requires("imgui", { 
    configs = {
        glfw = true,  -- 指定后端，可选sdl2、glut等
        opengl3 = true, -- 指定渲染API版本
        freetype = true,   -- 启用FreeType字体支持
        docking = true     -- 启用 docking 特性
    }
})

option("module_kind")
    set_default("static")
    set_showmenu(true)
    set_description("Build modules as shared or static (shared/static)")
    add_defines("CARROT_BUILD_SHARED")
option_end()
rule("utils.compile_shaders")
    after_build(function (target)
        local projdir = os.projectdir()
        local srcdir = path.join(projdir, "shaders")
        local outdir = path.join(target:targetdir(), "shaders")

        -- recreate output shaders dir
        os.rm(outdir)
        os.mkdir(outdir)

        -- locate dxc
        local dxc_path = nil
        local pkg = target:pkg("directxshadercompiler")
        if pkg then
            local installdir = pkg:installdir()
            if installdir and os.isdir(installdir) then
                local pattern = is_plat("windows") and "dxc*.exe" or "dxc*"
                for _, p in ipairs(os.files(path.join(installdir, "**", pattern))) do
                    if os.isfile(p) then
                        dxc_path = p
                        break
                    end
                end
            end
        end

        if not dxc_path then
             print("warning: directxshadercompiler package not found!")
        else
             print("using dxc from package: " .. dxc_path)
        end

        import("core.base.option")
        for _, f in ipairs(os.files(path.join(srcdir, "**"))) do
            if os.isfile(f) then
                local name = path.filename(f)
                local lname = name:lower()

                if lname:sub(-5) == '.hlsl' then
                    local basename = name:gsub("%.hlsl$", "")
                    local stage = "ps_6_0"
                    local entry = "PSMain"
                    if basename:lower():find('%.vs$') or basename:lower():find('%.vert$') then
                        stage = "vs_6_0"
                        entry = "VSMain"
                    elseif basename:lower():find('%.ps$') or basename:lower():find('%.frag$') then
                        stage = "ps_6_0"
                        entry = "PSMain"
                    end

                    local outspv = path.join(outdir, basename .. ".spv")

                    if dxc_path then
                        local cmd = string.format('"%s" -T %s -E %s -spirv -Fo "%s" "%s"', dxc_path, stage, entry, outspv, f)
                        print('> ' .. cmd)
                        local out = nil
                        try {
                            function () out = os.iorun(cmd) end,
                            catch { function (e) out = tostring(e) end }
                        }
                        if not os.isfile(outspv) then
                            print('warning: dxc failed for ' .. f .. ', copying original')
                            os.cp(f, outdir)
                        end
                    else
                        os.cp(f, outdir)
                    end
                else
                    os.cp(f, outdir)
                end
            end
        end
    end)
rule_end()

includes("src/Runtime/Core")
includes("src/Runtime/RHI")
includes("src/Runtime/Renderer")
includes("src/Runtime/Launch")
includes("src/Tools/Editor")
includes("src/DefaultGame")
includes("src/TestRHIApp")
includes("src/CustomModule")
