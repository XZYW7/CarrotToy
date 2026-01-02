// default.ps.hlsl - pixel shader (PBR approximated)

cbuffer Material : register(b1)
{
    float3 albedo;
    float metallic;
    float roughness;
}

cbuffer LightData : register(b2)
{
    float3 lightPos;
    float3 lightColor;
    float3 viewPos;
}

struct PSInput {
    float4 pos : SV_POSITION;
    float3 FragPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};

static const float PI = 3.14159265358979323846;

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(saturate(1.0 - cosTheta), 5.0);
}

float4 PSMain(PSInput input) : SV_Target
{
    float3 N = normalize(input.Normal);
    float3 V = normalize(viewPos - input.FragPos);

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);

    float3 L = normalize(lightPos - input.FragPos);
    float3 H = normalize(V + L);
    float distance = length(lightPos - input.FragPos);
    float attenuation = 1.0 / (distance * distance);
    float3 radiance = lightColor * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - metallic);

    float3 numerator = NDF * G * F;
    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    float3 specular = numerator / denom;

    float NdotL = max(dot(N, L), 0.0);
    float3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    float3 ambient = float3(0.03, 0.03, 0.03) * albedo;
    float3 color = ambient + Lo;

    // tone mapping
    color = color / (color + float3(1.0,1.0,1.0));
    // gamma
    color = pow(color, float3(1.0/2.2,1.0/2.2,1.0/2.2));

    return float4(color, 1.0);
}
