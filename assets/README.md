# Assets Directory

This directory is reserved for asset files used by CarrotToy.

## Planned Asset Types

- **Textures**: Material texture files (diffuse, normal, roughness, etc.)
- **Models**: 3D model files for preview (OBJ, FBX)
- **HDR Images**: Environment maps for IBL (Image-Based Lighting)
- **Material Presets**: Saved material configurations (JSON)
- **Shader Presets**: Common shader templates

## Directory Structure (Future)

```
assets/
├── textures/
│   ├── metal/
│   ├── fabric/
│   └── stone/
├── models/
│   ├── sphere.obj
│   ├── cube.obj
│   └── bunny.obj
├── hdri/
│   ├── studio.hdr
│   └── outdoor.hdr
├── materials/
│   ├── gold.json
│   └── plastic.json
└── shaders/
    ├── pbr_template.glsl
    └── toon_template.glsl
```

## Usage

Asset files will be loaded by the application and can be referenced in materials.

Example (future):
```cpp
material->setTexture("albedoMap", "assets/textures/metal/rusty_metal_albedo.png");
material->setTexture("normalMap", "assets/textures/metal/rusty_metal_normal.png");
```

## Contributing Assets

When contributing assets:
1. Ensure you have rights to the asset
2. Include attribution if required
3. Use appropriate file formats
4. Optimize file sizes
5. Update this README

## Supported Formats (Future)

- **Images**: PNG, JPG, TGA, HDR, EXR
- **Models**: OBJ, FBX, GLTF
- **Materials**: JSON
- **Shaders**: GLSL

## Credits

Assets will be credited here when added.
