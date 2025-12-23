# CarrotToy Tutorial: Creating Your First Material

This tutorial will guide you through creating and editing materials in CarrotToy, from basic setup to advanced shader customization.

## Prerequisites

- CarrotToy built and installed (see [BUILD.md](BUILD.md))
- Basic understanding of graphics programming concepts
- Familiarity with GLSL (helpful but not required)

## Tutorial 1: Getting Started

### Step 1: Launch CarrotToy

```bash
cd CarrotToy
xmake run CarrotToy
```

You should see:
- A window with a rotating sphere
- Several UI panels (Materials, Material Properties, Preview)

### Step 2: Explore the Default Material

1. Look at the **Materials** panel on the left
2. You should see "DefaultPBR" material listed
3. Click on it to select it
4. The sphere in the center should update to show this material

### Step 3: Modify Material Parameters

In the **Material Properties** panel:

1. Find the "albedo" color picker
2. Click on it and change the color
3. Watch the sphere update in real-time
4. Try adjusting:
   - **Metallic**: 0.0 (plastic) to 1.0 (metal)
   - **Roughness**: 0.0 (smooth/shiny) to 1.0 (rough/matte)

**Expected Result**: The sphere's appearance changes based on your parameter adjustments.

## Tutorial 2: Creating a Custom Material

### Step 1: Create a Gold Material

1. Click "Create New Material" in the Materials panel
2. Enter name: "Gold"
3. Click "Create"
4. Select the "Gold" material from the list

### Step 2: Configure Gold Properties

Set the following parameters:
```
Albedo:    R=1.0, G=0.85, B=0.57  (golden color)
Metallic:  1.0                     (full metal)
Roughness: 0.2                     (slightly rough)
```

**Expected Result**: A shiny, golden sphere.

### Step 3: Create a Rubber Material

Repeat the process with these values:
```
Name:      Rubber
Albedo:    R=0.1, G=0.1, B=0.1   (dark gray)
Metallic:  0.0                    (non-metallic)
Roughness: 0.9                    (very rough)
```

**Expected Result**: A matte, dark sphere with minimal reflections.

## Tutorial 3: Editing Shaders in Real-Time

### Step 1: Open the Shader Editor

1. Select a material (e.g., "DefaultPBR")
2. Click "Edit Shader" in Material Properties
3. The Shader Editor window opens

### Step 2: Examine the Shader Code

You'll see two text areas:
- **Vertex Shader**: Transforms vertex positions
- **Fragment Shader**: Computes pixel colors

### Step 3: Modify the Fragment Shader

Let's create a simple toon shader effect. Replace the fragment shader with:

```glsl
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 albedo;

void main()
{
    // Simple light direction
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 N = normalize(Normal);
    
    // Calculate lighting
    float NdotL = max(dot(N, lightDir), 0.0);
    
    // Toon shading - quantize to 3 levels
    float toon = NdotL;
    if (toon > 0.95) toon = 1.0;
    else if (toon > 0.5) toon = 0.6;
    else if (toon > 0.25) toon = 0.3;
    else toon = 0.1;
    
    vec3 color = albedo * toon;
    FragColor = vec4(color, 1.0);
}
```

### Step 4: Compile and Apply

1. Click "Compile and Apply"
2. Check the console for any errors
3. If successful, the sphere now has a toon-shaded appearance

**Expected Result**: The sphere has distinct lighting bands instead of smooth gradients.

## Tutorial 4: Creating Animated Materials

### Step 1: Add Time Uniform

Modify the fragment shader to include animation:

```glsl
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 albedo;
uniform float time;  // Add this

void main()
{
    vec3 N = normalize(Normal);
    
    // Animated pattern
    float pattern = sin(FragPos.x * 10.0 + time) * 
                   cos(FragPos.y * 10.0 + time);
    pattern = pattern * 0.5 + 0.5;  // Remap to 0-1
    
    vec3 color = albedo * pattern;
    FragColor = vec4(color, 1.0);
}
```

### Step 2: Set Time in Application

The time uniform is automatically updated in the main loop, so your animation should work immediately.

**Expected Result**: An animated wave pattern on the sphere.

## Tutorial 5: Creating Material Presets

### Preset 1: Chrome

```
Name:      Chrome
Albedo:    R=0.95, G=0.95, B=0.95
Metallic:  1.0
Roughness: 0.05
```

### Preset 2: Plastic

```
Name:      RedPlastic
Albedo:    R=0.9, G=0.1, B=0.1
Metallic:  0.0
Roughness: 0.4
```

### Preset 3: Copper

```
Name:      Copper
Albedo:    R=0.95, G=0.64, B=0.54
Metallic:  1.0
Roughness: 0.3
```

### Preset 4: Ceramic

```
Name:      Ceramic
Albedo:    R=0.9, G=0.9, B=0.85
Metallic:  0.0
Roughness: 0.1
```

## Tutorial 6: Understanding PBR Parameters

### Albedo (Base Color)

- Represents the surface color
- For metals: Use measured metal colors
- For dielectrics: Use actual surface color
- Avoid very dark (<30) or very bright (>240) values

**Common Materials:**
- Iron: RGB(198, 198, 198)
- Gold: RGB(255, 217, 145)
- Copper: RGB(242, 163, 138)
- Plastic: Any color, typically vibrant

### Metallic

- 0.0 = Non-metal (dielectric)
- 1.0 = Metal (conductor)
- Rarely use values in between
- Affects how the material reflects light

**Rule of thumb:**
- Plastic, rubber, ceramic = 0.0
- All metals = 1.0
- Oxidized/dirty metal = 0.9-1.0

### Roughness

- 0.0 = Perfect mirror
- 1.0 = Completely diffuse
- Controls light scattering

**Common Materials:**
- Polished metal: 0.1-0.3
- Worn metal: 0.5-0.7
- Matte plastic: 0.6-0.8
- Rough surface: 0.8-1.0

## Tutorial 7: Advanced Shader Techniques

### Normal Mapping Effect (Without Textures)

```glsl
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 albedo;

void main()
{
    vec3 N = normalize(Normal);
    
    // Procedural bump
    float bumpX = sin(FragPos.x * 50.0) * 0.1;
    float bumpY = sin(FragPos.y * 50.0) * 0.1;
    N.x += bumpX;
    N.y += bumpY;
    N = normalize(N);
    
    // Simple lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(N, lightDir), 0.0);
    
    vec3 color = albedo * (diff + 0.2);
    FragColor = vec4(color, 1.0);
}
```

### Rim Lighting Effect

```glsl
#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 albedo;

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(vec3(0.0, 0.0, 3.0) - FragPos);
    
    // Rim light
    float rim = 1.0 - max(dot(V, N), 0.0);
    rim = pow(rim, 3.0);
    
    vec3 rimColor = vec3(0.5, 0.7, 1.0) * rim;
    
    // Base lighting
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    float diff = max(dot(N, lightDir), 0.0);
    
    vec3 color = albedo * diff + rimColor;
    FragColor = vec4(color, 1.0);
}
```

## Tutorial 8: Using the Ray Tracer

### Step 1: Set Up Scene

1. Create and configure your materials
2. Select the material you want to ray trace

### Step 2: Export Scene

1. Go to the **Material Preview** panel
2. Click "Export Scene for Ray Tracing"
3. The scene is saved to `scene_export.txt`

### Step 3: Render with Ray Tracing

Currently, ray tracing is performed programmatically:

```cpp
CarrotToy::RayTracer tracer;
tracer.loadScene("scene_export.txt");
tracer.render(1920, 1080, "output.png");
```

**Expected Result**: A high-quality ray-traced image saved as `output.png`.

## Troubleshooting

### Issue: Shader won't compile

**Solution**: Check console for error messages. Common issues:
- Missing semicolons
- Undefined uniform variables
- Version mismatch (use `#version 330 core`)

### Issue: Material appears black

**Solution**:
- Check if albedo is set (shouldn't be 0,0,0)
- Verify lighting calculations in shader
- Ensure normals are being passed correctly

### Issue: UI is not responding

**Solution**:
- Restart the application
- Check if ImGui initialized correctly
- Verify GLFW window events are being processed

## Best Practices

1. **Start Simple**: Begin with basic parameters, then add complexity
2. **Test Frequently**: Compile shaders often to catch errors early
3. **Save Your Work**: Export materials before closing
4. **Experiment**: Try extreme parameter values to understand behavior
5. **Reference Real Materials**: Use real-world PBR values as reference

## Next Steps

- Explore the [EXAMPLES.md](EXAMPLES.md) for more code examples
- Read [ARCHITECTURE.md](ARCHITECTURE.md) to understand the system
- Contribute your own shader presets
- Extend the system with new features

## Additional Resources

### Learning PBR
- "Physically Based Rendering: From Theory to Implementation"
- LearnOpenGL PBR tutorial
- Unreal Engine PBR documentation

### GLSL Resources
- The Book of Shaders (thebookofshaders.com)
- Shadertoy (shadertoy.com)
- OpenGL Shading Language specification

### Community
- Create issues on GitHub for questions
- Share your materials and shaders
- Contribute improvements back to the project

Happy material editing!
