# CarrotToy Quick Start Guide

## 1-Minute Setup

```bash
# Install Xmake
curl -fsSL https://xmake.io/shget.text | bash

# Clone and build
git clone https://github.com/XZYW7/CarrotToy.git
cd CarrotToy
xmake
xmake run CarrotToy
```

## What You'll See

When you run CarrotToy, you'll see:

```
┌────────────────────────────────────────────────────────────┐
│  CarrotToy - Material Editor                               │
├──────────┬─────────────────────────┬────────────────────────┤
│          │                         │  Material Properties   │
│ Materials│     [Rotating Sphere]   │  ├─ Albedo: [RGB]     │
│ List     │     with Current        │  ├─ Metallic: 0.5     │
│          │     Material            │  └─ Roughness: 0.5    │
│ DefaultPBR│                        │  [Edit Shader]         │
│ Gold     │                         │                        │
│ Plastic  │                         │  Preview Window        │
│          │                         │  ○ Rasterization       │
│[+ New]   │                         │  ○ Ray Tracing         │
└──────────┴─────────────────────────┴────────────────────────┘
```

## First Steps

### 1. Modify a Material (10 seconds)
- Click "DefaultPBR" in Materials panel
- Drag the "albedo" color picker
- Watch the sphere change color!

### 2. Create New Material (30 seconds)
- Click "Create New Material"
- Name it "Gold"
- Set: Albedo (1.0, 0.85, 0.57), Metallic: 1.0, Roughness: 0.2
- You now have a gold material!

### 3. Edit Shaders Live (2 minutes)
- Select a material
- Click "Edit Shader"
- Modify the fragment shader code
- Click "Compile and Apply"
- See instant results!

## Key Features in 30 Seconds

✅ **Real-time editing** - Change materials and see results instantly  
✅ **Live shader editing** - Edit GLSL code without restarting  
✅ **PBR materials** - Physically-based rendering out of the box  
✅ **Ray tracing** - Export and render high-quality images  
✅ **Easy to extend** - Clean architecture, well documented  

## Common Tasks

### Create a Shiny Metal
```
Albedo: Any color (e.g., 0.8, 0.8, 0.8 for silver)
Metallic: 1.0
Roughness: 0.1
```

### Create Matte Plastic
```
Albedo: Any color
Metallic: 0.0
Roughness: 0.7
```

### Add Simple Animation
In shader editor, add:
```glsl
uniform float time;
// In main():
vec3 color = albedo * (sin(time) * 0.5 + 0.5);
```

## Need Help?

- 📖 [Full Tutorial](TUTORIAL.md) - Step-by-step guide
- 📚 [Examples](EXAMPLES.md) - Code examples
- 🏗️ [Architecture](ARCHITECTURE.md) - System design
- 🔧 [Build Guide](BUILD.md) - Detailed build instructions

## Next Steps

1. ✏️ Create your first custom material
2. 🎨 Experiment with shader code
3. 📸 Try ray tracing mode
4. 🚀 Extend with new features

**Start creating amazing materials now!** 🎉
