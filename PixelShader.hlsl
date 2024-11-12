#include "ShaderStructs.hlsli"
#include "ShaderFunctions.hlsli"

cbuffer DataFromCPU : register(b0) // Take the data from memory register b0 ("buffer 0")
{
    float4 colorTint;
	float roughness;
    float2 uvScale;
    float2 uvOffset;
	float3 cameraLocation;
	float3 ambient;

	int lightCount;
	Light lights[64];
}

// Set of options for sampling
SamplerState BasicSampler : register(s0);

// Define textures
Texture2D SurfaceColorTexture : register(t0);
Texture2D SpecularMap : register(t1);
Texture2D NormalMap : register(t2);

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
    // Apply UV transformations based on material settings
    input.uv *= uvScale;
    input.uv += uvOffset;

	float3 textureColor = SurfaceColorTexture.Sample(BasicSampler, input.uv + uvOffset).rgb * colorTint.rgb;
	float specularScale = SpecularMap.Sample(BasicSampler, input.uv + uvOffset).r;
    float3 unpackedNormal = normalize(NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1);

    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    tangent = normalize(tangent - normal * dot(normal, tangent)); // Orthonormalize normal and tangent using Gram-Schmidt Process
    float3 bitangent = cross(normal, tangent);
    float3x3 rotationMatrix = float3x3(tangent, bitangent, normal);

	input.normal = mul(unpackedNormal, rotationMatrix);

    float3 diffuseColor = 0;
    float3 specularColor = 0;
	for(int i = 0; i < lightCount; i++)
    {
		// Diffuse
        diffuseColor += diffuse(input, lights[i]);

	    // Specular
        specularColor += specular(input, lights[i], cameraLocation, roughness) * specularScale;
    }

    return textureColor.rgbb * float4(ambient + diffuseColor + specularColor, 1);
}