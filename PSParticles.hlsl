#include "Particles.hlsli"

cbuffer ExternalData : register(b0) // Take the data from memory register b0 ("buffer 0")
{
    float4 colorTint;
    float2 uvScale;
    float2 uvOffset;
}

SamplerState BasicSampler : register(s0);

Texture2D AlbedoTexture : register(t0);

float4 main(VertexToPixel_Particle input) : SV_TARGET
{
    // Apply UV transformations based on material settings
    input.uv *= uvScale;
    input.uv += uvOffset;
    
    // Sample textures
    float4 textureColor = pow(AlbedoTexture.Sample(BasicSampler, input.uv) * colorTint, 2.2f); // Reverse gamma correction of surface texture - final color is re-corrected
    
    return pow(textureColor * colorTint, 1.0f / 2.2f) * input.color; // Gamma correct the final result
}