#include "ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"

cbuffer DataFromCPU : register(b0) // Take the data from memory register b0 ("buffer 0")
{
    float4 colorTint;
	float roughness;
	float3 cameraLocation;
	float3 ambient;

	int lightCount;
	Light lights[64];
}

// Define textures
Texture2D SurfaceColorTexture : register(t0);

// Set of options for sampling
SamplerState BasicSampler : register(s0);

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
	float4 colorFromTexture = SurfaceColorTexture.Sample(BasicSampler, input.uv);
	return colorFromTexture;

	input.normal = normalize(input.normal);

    float3 diffuseColor = 0;
    float3 specularColor = 0;
	for(int i = 0; i < lightCount; i++)
    {
		// Diffuse
        diffuseColor += diffuse(input, lights[i]);

	    // Specular
        specularColor += specular(input, lights[i], cameraLocation, roughness);
    }

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    return colorTint * float4(ambient + diffuseColor + specularColor, 1);
}