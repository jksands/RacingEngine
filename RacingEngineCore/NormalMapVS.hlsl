#include "ShaderIncludes.hlsli"

// Constant Buffer
cbuffer externalData : register(b0)
{
    float4 tint;
    matrix world;
    matrix view;
    matrix proj;
};

// Entry point
V2P_NormalMap main(VertexShaderInput input)
{
	// Set up output struct
    V2P_NormalMap output;
	
	// I am NOT transposing the matrices, so multiplication order is reversed
	// -- PVM instead of MVP
    matrix worldViewProj = mul(proj, mul(view, world));

	// Apply MVP matrix to position
    output.position = mul(worldViewProj, float4(input.position, 1.0f));

	// Pass the color through 
    output.color = tint;
	
	// the 3x3 portion of the world matrix contains all our transformation data
	// We don't need translation since it is a vector
	// ONLY WORKS FOR UNIFORM SCALE
	// -- if want to work with variable scale, use transpose of this matrix passed in from C++
    output.normal = mul((float3x3) world, input.normal);
	
	// Pass Tangent through, but rotate it first
    output.tangent = (float3) normalize(mul((float3x3) world, input.tangent));
	
	// Calculate world position
    output.worldPos = (float3) mul(world, float4(input.position, 1));
	
    output.uv = input.uv;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
    return output;
}