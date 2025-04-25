#include "Particles.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix viewMatrix;
    matrix projMatrix;
}

StructuredBuffer<Particle> ParticlePool : register(t0);
StructuredBuffer<uint> DrawList : register(t1);

VertexToPixel_Particle main(uint id : SV_VertexID)
{
    VertexToPixel_Particle output;
    
    uint drawID = id / 4;
    uint cornerID = id % 4;
    
        uint particleID = DrawList.Load(drawID);
    Particle particle = ParticlePool.Load(particleID);
	
    float2 offset = float2(0, 0);
    float2 uv = float2(0, 0);
    switch(cornerID)
    {
        // Top-left
        case 0:
            offset = float2(-0.5f, 0.5f);
            uv = float2(0, 0);
            break;
        // Top-right
        case 1:
            offset = float2(0.5f, 0.5f);
            uv = float2(1, 0);
            break;
        // Bottom-right
        case 2:
            offset = float2(0.5f, -0.5f);
            uv = float2(1, 1);
            break;
        // Bottom-left
        case 3:
            offset = float2(-0.5f, -0.5f);
            uv = float2(0, 1);
            break;
        default: break;
    }
    
    float3 position = particle.location;
    
    // Handle rotation - get sin/cos and build a rotation matrix
    float s, c;
    sincos(particle.rotation, s, c);
    float2x2 rot =
    {
        c, s,
		-s, c
    };
    // Rotate the offset for this corner and apply size
    float2 rotatedOffset = mul(offset, rot) * 1;
    
	// Transform location based on camera's right and up vectors
    float3 cameraRight = float3(viewMatrix._11, viewMatrix._12, viewMatrix._13);
    float3 cameraUp = float3(viewMatrix._21, viewMatrix._22, viewMatrix._23);
    position += cameraRight * rotatedOffset.x;
    position += cameraUp * rotatedOffset.y;

	// Only use view and projection; particle.location is assumed to be in world space
    matrix viewProj = mul(projMatrix, viewMatrix);
    output.screenPosition = mul(viewProj, float4(position, 1.0f));
    
    output.uv = uv;
    
    output.color = particle.color;
    
	return output;
}