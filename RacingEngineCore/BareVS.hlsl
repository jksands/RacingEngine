struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 position : POSITION; // XYZ position
	// JUST POSITION
};

// Struct representing the data we're sending down the pipeline
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 position : SV_POSITION; // XYZW position (System Value Position)
	// JUST POSITIOn
};

// Constant Buffer
cbuffer externalData : register(b0)
{
    float4 tint;
    matrix world;
    matrix view;
    matrix proj;
};

// Entry point
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel output;
	
	// I am NOT transposing the matrices, so multiplication order is reversed
	// -- PVM instead of MVP
    matrix worldViewProj = mul(proj, mul(view, world));

	// Apply MVP matrix to position
    output.position = mul(worldViewProj, float4(input.position, 1.0f));
	

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
    return output;
}