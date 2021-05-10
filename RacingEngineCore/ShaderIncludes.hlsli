#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 position     : POSITION; // XYZ position
    float3 normal       : NORMAL; // Vertex Normal
    float3 tangent      : TANGENT; // Vertex Tangent
    float2 uv           : TEXCOORD; // UV coords
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 position     : SV_POSITION; // XYZW position (System Value Position)
	float4 shPos		: POSITION1;	// XYZW position (System Value Position)
    float4 color        : COLOR; // RGBA color
    float3 normal       : NORMAL; // Vertex normal
    float3 worldPos     : POSITION0; // world position
    float2 uv           : TEXCOORD; // uv coords
};

// Vertex to Pixel struct solely for Normal mapping
struct V2P_NormalMap
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 position     : SV_POSITION; // XYZW position (System Value Position)
	float4 shPos		: POSITION1;	// XYZW position (System Value Position)
    float4 color        : COLOR; // RGBA color
    float3 normal       : NORMAL; // Vertex normal
    float3 tangent      : TANGENT;
    float3 worldPos     : POSITION0; // world position
    float2 uv           : TEXCOORD; // uv coords
};

// Represents a directional light
struct DirectionalLight
{
    float3 ambient;
    float3 diffuse;
    float3 direction;
};


// Calculates light for the pixel shader
float3 calcLight(float3 amt, DirectionalLight l, float3 sColor, float3 n, float3 v, float s)
{
    float3 r;
    r = reflect(normalize(l.direction), n);
    
    float specCalc = s * pow(saturate(dot(r, v)), 256);
    specCalc *= any(amt);
    
    return amt * l.diffuse * sColor + l.ambient * sColor + specCalc;
}
#endif