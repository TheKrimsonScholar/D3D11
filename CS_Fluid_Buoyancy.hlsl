cbuffer ExternalData : register(b0)
{
    float deltaTime;
};

Texture3D<float4> VelocityPrevious : register(t0);
Texture3D<float> ReactionPrevious : register(t1);

RWTexture3D<float4> VelocityCurrent : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float buoyancy = 1.0f;
    
    float3 velocity = VelocityPrevious[DTid].xyz;
    velocity.y += ReactionPrevious[DTid] * buoyancy * deltaTime;
    
    VelocityCurrent[DTid] = float4(velocity, 1);
}