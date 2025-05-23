#include "Fluids.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix worldMatrix;
    matrix worldInverseMatrix;
    
    float3 cameraLocation;
};

Texture3D<float> ReactionPrevious : register(t0);

SamplerState Sampler : register(s0);

float4 main(VertexToPixel_Fluid input) : SV_TARGET
{
    static const float3 BOX_NORMALS[6] =
    {
        float3(-1, 0, 0), // Left
        float3(1, 0, 0), // Right
        float3(0, -1, 0), // Down
        float3(0, 1, 0), // Up
        float3(0, 0, -1), // Backward
        float3(0, 0, 1), // Forward
    };
    
    float3 rayOrigin = cameraLocation;
    float3 rayDirection = normalize(input.worldPosition - cameraLocation);
	
	/* Ray-OBB Intersection */
	
    float tMin = 0, tMax = 1000000;
    float3 center = mul(worldMatrix, float4(0, 0, 0, 1));
    for(uint i = 0; i < 6; i++)
    {
        // Define the plane of this face in world space
        float3 p = mul(worldMatrix, float4(BOX_NORMALS[i], 1)); // Arbitrary point on the plane
        float3 normal = normalize(p - center); // Plane normal
        float offset = dot(normal, p); // Plane offset

        float t = (offset - dot(normal, rayOrigin)) / dot(normal, rayDirection); // Intersection coordinate
        // If the ray is in the same direction as the normal, t is an upper bound (exit point)
        if(dot(rayDirection, normal) > 0)
            tMax = min(tMax, t);
        // If the ray is in the opposite direction of the normal, t is a lower bound (entry point)
        else
            tMin = max(tMin, t);
    }
    
    /* Ray-marching */
    
    float dt = 0.01f; // Arbitrary sample rate
    float4 totalColor = float4(0.2f, 0.2f, 0.2f, 0);
    for(float t = tMin; t < tMax; t += dt)
    {
        // Sample the fluid density at this point
        float3 worldPosition = rayOrigin + rayDirection * t;
        float3 samplePosition = mul(worldInverseMatrix, float4(worldPosition, 1)); // Convert to grid coordinates (-0.5f to 0.5f)
        samplePosition /= 2;
        samplePosition += float3(0.5f, 0.5f, 0.5f); // Normalized UVW coordinates
        
        float reactionCoordinate = ReactionPrevious.SampleLevel(Sampler, samplePosition, 0);

        float fireAmount = saturate((reactionCoordinate - 0.5f) * 2.0f);
        float3 fireColor = lerp(float3(1, 0.5f, 0), float3(1, 0, 0), fireAmount); // Lerp between orange and red

        if(fireAmount > 0)
            totalColor += float4(fireAmount * fireColor, (tMax - tMin) / dt);
    }
	
	return totalColor;
}