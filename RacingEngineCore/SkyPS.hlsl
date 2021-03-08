TextureCube EnvironmentMap : register(t0); // "t" registers
SamplerState samplerOptions : register(s0); // "s registers

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : TEXCOORD0;
};




// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    return EnvironmentMap.Sample(samplerOptions, input.sampleDir);
}

