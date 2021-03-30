#include "ShaderIncludes.hlsli"

// Entry point
float4 main(VertexToPixel input) : SV_TARGET
{
    return input.color;
}