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

// Entry point
float4 main(VertexToPixel input) : SV_TARGET
{
    return input.position;
}