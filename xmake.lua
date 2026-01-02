-- CarrotToy - Material Editor Lab
-- Xmake build configuration

set_project("CarrotToy")
set_version("0.1.0")
set_languages("c++17")

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
target("CarrotToy")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("include/*.h")
    add_includedirs("include")
    
    -- Link required packages
    add_packages("glfw", "glad", "glm", "imgui", "stb", "directxshadercompiler")
    
    -- Platform-specific settings
    if is_plat("windows") then
        add_syslinks("opengl32", "gdi32", "user32", "shell32")
    elseif is_plat("linux") then
        add_syslinks("GL", "pthread", "dl", "X11")
    elseif is_plat("macosx") then
        add_frameworks("OpenGL", "Cocoa", "IOKit", "CoreVideo")
    end
    
    after_build(function (target)
        local projdir = os.projectdir()
        local srcdir = path.join(projdir, "shaders")
        local outdir = path.join(target:targetdir(), "shaders")

        -- recreate output shaders dir
        os.rm(outdir)
        os.mkdir(outdir)

        -- helper: get filename only
        local function filename(p)
            local n = p:match("[^/\\]+$")
            return n or p
        end
        -- locate dxc: prefer xmake-installed package 'directxshadercompiler'
        local dxc_path = nil
        -- Try to find directxshadercompiler in target packages
        local installdir = target:pkgs()["directxshadercompiler"]:installdir()
        if installdir and os.isdir(installdir) then
            local pattern = is_plat("windows") and "dxc*.exe" or "dxc*"
            for _, p in ipairs(os.files(path.join(installdir, "**", pattern))) do
                if os.isfile(p) then
                    dxc_path = p
                    break
                end
            end
        end

        if not dxc_path then
             print("warning: directxshadercompiler package not found in target packages!")
        else
             print("using dxc from package: " .. dxc_path)
        end

        for _, f in ipairs(os.files(path.join(srcdir, "**"))) do
            if os.isfile(f) then
                local name = filename(f)
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
                        -- run safely, capture output; judge success by produced file
                        local out = nil
                        try {
                            function () out = os.iorun(cmd) end,
                            catch { function (e) out = tostring(e) end }
                        }
                        if not os.isfile(outspv) then
                            print('warning: dxc failed for ' .. f .. ' (output: ' .. (out or "<no output>") .. '), copying original file')
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
    
    set_targetdir("build/bin")
    set_objectdir("build/obj")
target_end()
