#include "ShaderIncludes.hlsli"

Texture2D diffuseTexture           : register(t0); // "t" registers
SamplerState samplerOptions   : register(s0); // "s" registers

// Constant buffer
cbuffer lightData : register (b0)
{
    DirectionalLight directionalLight;
    float3 camPos;
    float spec;
}

// Entry point
float4 main(VertexToPixel input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    // View vector
    float3 v = normalize(camPos - (float3) input.worldPos);
	
	// Calculate direction TO the light
    float3 dir = normalize(-directionalLight.direction);
	
	// Calculate light amount (N dot L)
    float3 lightAmt = saturate(dot(normal, dir));
    
    // Pull the surface color from the texture
    float3 surfaceColor = diffuseTexture.Sample(samplerOptions, input.uv);
	
	// calculate final pixel color
    float3 finalColor = calcLight(lightAmt, directionalLight, surfaceColor + (float3)input.color, normal, v, spec);
	
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    return float4(finalColor, 1);
}