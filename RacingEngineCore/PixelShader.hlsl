#include "ShaderIncludes.hlsli"

Texture2D diffuseTexture           : register(t0); // "t" registers
Texture2D ShadowMap : register(t1);
SamplerState samplerOptions   : register(s0); // "s" registers
SamplerComparisonState shadowSampler : register(s1); 

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
    // float3 surfaceColor = 0;
    
     // Begin Shadow calcs
    // calculate depth (distance) from light
    // "w" divides only necessary for perspective proj.
    float lightDepth = input.shPos.z / input.shPos.w;
    
    // Adjust [-1 to 1] range to be [0 to 1] for UV's
    float2 shadowUV =
        input.shPos.xy / input.shPos.w * 0.5f + 0.5f;
    
    // Flip y (UV's "y" upside down form screen)
    shadowUV.y = 1.0f - shadowUV.y;
    
    // Read shadow Map for closest surface (red channel)
    // float shDepth =
    //     ShadowMap.Sample(shadowSampler, shadowUV).r; // This will be "shadow or not" value
    float shadowAmount = ShadowMap.SampleCmpLevelZero(shadowSampler, shadowUV, lightDepth);
	
	// calculate final pixel color
    float3 finalColor = calcLight(lightAmt, directionalLight, surfaceColor + (float3)input.color, normal, v, spec);
    finalColor *= shadowAmount;
    // saturate(finalColor);
	
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    return float4(finalColor, 1);
}