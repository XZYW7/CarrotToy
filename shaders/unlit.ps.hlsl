// unlit.ps.hlsl

cbuffer MaterialColor : register(b1)
{
    float3 color;
}

struct PSInput {
    float4 pos : SV_POSITION;
    float3 FragPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};

float4 PSMain(PSInput input) : SV_Target
{
    return float4(color, 1.0);
}
