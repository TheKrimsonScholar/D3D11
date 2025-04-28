#include "Fluids.hlsli"

cbuffer ExternalData : register(b0)
{
    matrix worldMatrix;
    matrix worldInverseMatrix;
    
    float3 cameraLocation;
};

Texture3D<float> DensityPrevious : register(t0);
Texture3D<float> TemperaturePrevious : register(t1);

SamplerState Sampler : register(s0);

float4 main(VertexToPixel_Fluid input) : SV_TARGET
{
    //return float4(0, 0, 0, 0);
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
	
    //return float4(1, 1, 1, 1);
    
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
    
    //return float4(tMax / 4.0f, 0, 0, 1);
    float dt = 0.01f;
    //dt = (tMax - tMin) / 10.0f;
    //return float4(DensityPrevious[uint3(0, 0, 0)], 0, 0, 1);
    //return float4(DensityPrevious.SampleLevel(Sampler, float3(0, 0, 0), 0), 0, 0, 1);
    //return float4(dt, 0, 0, 1);
    float4 totalColor = float4(0, 0, 0, 0);
    for(float t = tMin; t < tMax; t += dt)
    {
        // Sample the fluid density at this point
        float3 worldPosition = rayOrigin + rayDirection * t;
        float3 samplePosition = mul(worldInverseMatrix, float4(worldPosition, 1)); // Convert to grid coordinates (-0.5f to 0.5f)
        samplePosition /= 2;
        samplePosition += float3(0.5f, 0.5f, 0.5f); // Normalized UVW coordinates
        
        //float density = DensityPrevious.SampleLevel(Sampler, samplePosition, 0);
        
        // If the density is above a certain threshold, we have hit the fluid
        //if(density > 0.25f)
            //return float4(t / tMax, 1.0f, 1.0f, 1.0f);
        
        //totalColor += float4(density, density, density, 1) * dt;
        
        float density = DensityPrevious.SampleLevel(Sampler, samplePosition, 0);
        float temperature = TemperaturePrevious.SampleLevel(Sampler, samplePosition, 0);

        float fireAmount = saturate((temperature - 0.5) * 2.0);
        float smokeAmount = density * (1.0 - fireAmount);

        float3 fireColor = lerp(float3(1, 0.5, 0), float3(1, 0, 0), fireAmount);
        float3 smokeColor = float3(0.2, 0.2, 0.2);

        totalColor += float4(fireAmount * fireColor + smokeAmount * smokeColor, 1);
    }
	
	return totalColor;
}