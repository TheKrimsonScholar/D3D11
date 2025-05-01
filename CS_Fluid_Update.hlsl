cbuffer ExternalData : register(b0)
{
    uint width;
    uint height;
    uint depth;
    
    float deltaTime;
};

Texture3D<float4> VelocityPrevious : register(t0);
Texture3D<float> ReactionPrevious : register(t1);

RWTexture3D<float4> VelocityCurrent : register(u0);
RWTexture3D<float> ReactionCurrent : register(u1);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    VelocityCurrent[DTid] = VelocityPrevious[DTid];
    
    /* Ignite sphere every frame */
    
    float3 uvw = DTid / float3(width, height, depth);
    float3 diff = uvw - float3(0.5f, 0.5f, 0.5f);
    if(length(diff) < 0.1f)
        ReactionCurrent[DTid] = 1.0f;
}