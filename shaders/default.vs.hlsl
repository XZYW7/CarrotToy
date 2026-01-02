// default.vs.hlsl - vertex shader

cbuffer PerFrame : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
}

struct VSInput {
    float3 aPos : POSITION;
    float3 aNormal : NORMAL;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float3 FragPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};

VSOutput VSMain(VSInput input)
{
    VSOutput o;
    float4 worldPos = mul(float4(input.aPos, 1.0), model);
    o.FragPos = worldPos.xyz;
    // normal transform: inverse transpose of model
    float3x3 normalMat = (float3x3)transpose(model);
    o.Normal = mul(input.aNormal, normalMat);
    o.pos = mul(mul(mul(float4(input.aPos, 1.0), model), view), projection);
    return o;
}
