#include "ShaderIncludes.hlsli"

Texture2D diffuseTexture : register(t0); // "t" registers
SamplerState samplerOptions : register(s0); // "s" registers

// Constant buffer
cbuffer lightData : register(b0)
{
    DirectionalLight directionalLight;
    float3 camPos;
    float spec;
}

// Entry point
float4 main(VertexToPixel input) : SV_TARGET
{
    return input.color;
}