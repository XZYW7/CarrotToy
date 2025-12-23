# CarrotToy Build Instructions

## Prerequisites

Before building CarrotToy, ensure you have the following installed:

1. **Xmake Build System**
   ```bash
   # Linux/macOS
   curl -fsSL https://xmake.io/shget.text | bash
   
   # Or via package managers
   # Ubuntu/Debian
   sudo add-apt-repository ppa:xmake-io/xmake
   sudo apt update
   sudo apt install xmake
   
   # macOS
   brew install xmake
   
   # Windows
   # Download installer from https://github.com/xmake-io/xmake/releases
   ```

2. **C++ Compiler**
   - GCC 7+ / Clang 5+ / MSVC 2017+ with C++17 support

3. **OpenGL Development Libraries**
   ```bash
   # Ubuntu/Debian
   sudo apt install libgl1-mesa-dev libglu1-mesa-dev
   
   # macOS - OpenGL is included in the system
   
   # Windows - OpenGL headers are typically included with the compiler
   ```

## Building CarrotToy

### Quick Start

```bash
# Clone the repository
git clone https://github.com/XZYW7/CarrotToy.git
cd CarrotToy

# Build the project (dependencies will be automatically downloaded)
xmake

# Run the application
xmake run CarrotToy
```

### Detailed Build Steps

1. **Configure the build**
   ```bash
   # Debug build (default)
   xmake f -m debug
   
   # Release build with optimizations
   xmake f -m release
   
   # Specify compiler
   xmake f --toolchain=gcc
   xmake f --toolchain=clang
   ```

2. **Build the project**
   ```bash
   xmake
   
   # Or build with verbose output
   xmake -v
   
   # Build with specific number of parallel jobs
   xmake -j4
   ```

3. **Run the application**
   ```bash
   xmake run CarrotToy
   
   # Or run the executable directly
   ./build/bin/CarrotToy
   ```

4. **Clean the build**
   ```bash
   # Clean build artifacts
   xmake clean
   
   # Clean all including configuration
   xmake clean -a
   ```

## Dependency Management

Xmake automatically manages the following dependencies:
- **GLFW** - Window and input handling
- **GLAD** - OpenGL function loading
- **GLM** - Mathematics library for graphics
- **ImGui** - Immediate mode GUI
- **STB** - Image loading and writing

These will be downloaded and built automatically on the first build.

## Platform-Specific Notes

### Linux

Ensure you have X11 development libraries:
```bash
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### macOS

On macOS, you may need to install Xcode Command Line Tools:
```bash
xcode-select --install
```

### Windows

On Windows, ensure you have:
- Visual Studio 2017 or later (for MSVC compiler)
- Or MinGW-w64 (for GCC on Windows)

## Troubleshooting

### Package Download Issues

If xmake cannot download packages:
```bash
# Use a mirror
xmake g --pkg_searchdirs=/path/to/local/packages

# Or manually specify package source
xmake f --pkg=glfw --pkg_searchdirs=/path/to/packages
```

### OpenGL Issues

If you get OpenGL-related errors:
1. Ensure your graphics drivers are up to date
2. Check OpenGL version support:
   ```bash
   glxinfo | grep "OpenGL version"  # Linux
   ```
3. CarrotToy requires OpenGL 3.3+

### Build Errors

If you encounter build errors:
1. Update xmake: `xmake update`
2. Clean and rebuild: `xmake clean -a && xmake`
3. Check compiler version: `gcc --version` or `clang --version`
4. Ensure C++17 support is available

## Development Build

For development with debug symbols and no optimizations:
```bash
xmake f -m debug
xmake
xmake run CarrotToy
```

## Release Build

For optimized release build:
```bash
xmake f -m release
xmake
xmake install -o /path/to/install/directory
```

## IDE Integration

### Visual Studio Code

Install the xmake-vscode extension:
1. Open VSCode
2. Install "xmake" extension
3. Open the CarrotToy folder
4. Use `Ctrl+Shift+P` and select "XMake: Configure"

### CLion

CLion supports xmake through the xmake plugin:
1. Install the xmake plugin in CLion
2. Open the CarrotToy project
3. CLion will automatically detect the xmake.lua file

## Testing the Build

After building, verify the installation:
```bash
# Check if the executable was created
ls build/bin/CarrotToy

# Run the application
xmake run CarrotToy
```

You should see:
- A window titled "CarrotToy - Material Editor"
- A rotating sphere with PBR material
- ImGui panels for material editing

## Additional Resources

- [Xmake Documentation](https://xmake.io/#/getting_started)
- [CarrotToy README](README.md)
- [OpenGL Wiki](https://www.khronos.org/opengl/wiki/)
