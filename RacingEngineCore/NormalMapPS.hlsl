#include "ShaderIncludes.hlsli"

Texture2D diffuseTexture    : register(t0); // "t" registers
Texture2D normalMap         : register(t1);

SamplerState samplerOptions : register(s0); // "s" registers

// Constant buffer
cbuffer lightData : register(b0)
{
    DirectionalLight directionalLight;
    float3 camPos;
    float spec;
}

// Entry point
float4 main(V2P_NormalMap input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    // View vector
    float3 v = normalize(camPos - (float3) input.worldPos);
    
    // normal map
    // Unpack the normal from the normalMap texture.
    float3 unpackedNormal = normalMap.Sample(samplerOptions, input.uv).rgb * 2 - 1;
    float3 N = normal;
    float3 T = normalize(input.tangent);
    T = normalize(T - N * dot(T, N));
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    normal = mul(normalize(unpackedNormal), TBN);
    
	
	// Calculate direction TO the light
    float3 dir = normalize(-directionalLight.direction);
	
	// Calculate light amount (N dot L)
    float3 lightAmt = saturate(dot(normal, dir));
    
    // Pull the surface color from the texture
    float3 surfaceColor = diffuseTexture.Sample(samplerOptions, input.uv);
	
	// calculate final pixel color
    float3 finalColor = calcLight(lightAmt, directionalLight, surfaceColor, normal, v, spec);
	
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    return float4(finalColor, 1);
}