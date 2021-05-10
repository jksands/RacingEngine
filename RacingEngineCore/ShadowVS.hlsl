// Defining a cbuffer to store data on GPU
// Structure must directly match between here and GPU
cbuffer ExternalData : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
}

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
    float3 position : POSITION;
};
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
    float4 position : SV_POSITION;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel output;
    matrix wvp = mul(proj, mul(view, world));
    
    // Calculate world position
    output.position = mul(wvp, float4(input.position, 1.0f));
	// Calculate the world position of the vertex
    // output.worldPos = mul(world, float4(input.position, 1.0f));
    
    return output;
}