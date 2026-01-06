#pragma once

#include <string>
#include <vector>

namespace CarrotToy {

// Ray Tracing utilities for offline rendering
class RayTracer {
public:
    struct Ray {
        float origin[3];
        float direction[3];
    };
    
    struct Hit {
        bool hit;
        float t;
        float position[3];
        float normal[3];
        void* material;
    };
    
    struct Scene {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        std::vector<void*> materials;
    };
    
    RayTracer();
    ~RayTracer();
    
    bool loadScene(const std::string& path);
    void render(int width, int height, const std::string& outputPath);
    
    Hit traceRay(const Ray& ray, const Scene& scene);
    void computeColor(const Hit& hit, float* color);
    
private:
    Scene scene;
    int maxBounces;
    int samplesPerPixel;
};

} // namespace CarrotToy
