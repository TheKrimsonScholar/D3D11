cbuffer ExternalData : register(b0)
{
    uint width;
    uint height;
    uint depth;
    
    float deltaTime;
};

Texture3D<float4> VelocityPrevious : register(t0);
Texture3D<float> PressurePrevious : register(t1);
Texture3D<float> DensityPrevious : register(t2);
Texture3D<float> ReactionPrevious : register(t3);

SamplerState Sampler : register(s0);

RWTexture3D<float4> VelocityCurrent : register(u0);
RWTexture3D<float> PressureCurrent : register(u1);
RWTexture3D<float> DensityCurrent : register(u2);
RWTexture3D<float> ReactionCurrent : register(u3);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 uvw = (DTid + 0.5f) / float3(width, height, depth); // UVW at the center of the voxel
    float3 velocity = VelocityPrevious.SampleLevel(Sampler, uvw, 0);
    float3 X = uvw - velocity * deltaTime;
    
    // Velocity
    VelocityCurrent[DTid] = VelocityPrevious.SampleLevel(Sampler, X, 0);
    
    // Pressure
    PressureCurrent[DTid] = PressurePrevious.SampleLevel(Sampler, X, 0);
    
    // Density
    DensityCurrent[DTid] = DensityPrevious.SampleLevel(Sampler, X, 0);
    
    // Reaction Coordinate
    ReactionCurrent[DTid] = ReactionPrevious.SampleLevel(Sampler, X, 0);
}