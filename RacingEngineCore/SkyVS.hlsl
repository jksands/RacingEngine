// Defining a cbuffer to store data on GPU
// Structure must directly match between here and GPU
cbuffer ExternalData : register(b0)
{
    matrix view;
    matrix proj;
}

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : TEXCOORD0;
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
    matrix viewNoTranslation = view;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    // calculate output position w/ just view and proj
    matrix viewProj = mul(proj, viewNoTranslation);
	// Here we're essentially passing the input position directly through to the next
	// stage (rasterizer), though it needs to be a 4-component vector now.  
	// - To be considered within the bounds of the screen, the X and Y components 
	//   must be between -1 and 1.  
	// - The Z component must be between 0 and 1.  
	// - Each of these components is then automatically divided by the W component, 
	//   which we're leaving at 1.0 for now (this is more useful when dealing with 
	//   a perspective projection matrix, which we'll get to in future assignments).
    output.position = mul(viewProj, float4(input.position, 1.0f));
    output.position.z = output.position.w;
	// Calculate the world position of the vertex
    // output.worldPos = mul(world, float4(input.position, 1.0f));

	// Pass the color through 
	// - The values will be interpolated per-pixel by the rasterizer
	// - We don't need to alter it here, but we do need to send it to the pixel shader
    // output.color = colorTint;

	// Alter the normal coming in based on tthe world matrix's transformations
    // output.normal = mul((float3x3) world, input.normal);

    output.sampleDir = input.position;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
    return output;
}