cbuffer ExternalData : register(b0)
{
    uint width;
    uint height;
    uint depth;
};

RWTexture3D<float4> VelocityCurrent : register(u0);
RWTexture3D<float> PressureCurrent : register(u1);
RWTexture3D<float> DensityCurrent : register(u2);
RWTexture3D<float> ReactionCurrent : register(u3);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{   
    float3 uvw = DTid / float3(width, height, depth);
    float3 diff = uvw - float3(0.5f, 0.5f, 0.5f);
    
    /* Initialize values in a spherical region */
    
    VelocityCurrent[DTid] = float4(0, 0, 0, 0);
    if(length(diff) < 0.1f)
    {
        PressureCurrent[DTid] = 1.0f;
        DensityCurrent[DTid] = 1.0f;
        ReactionCurrent[DTid] = 1.0f;
    }
    else
    {
        PressureCurrent[DTid] = 0.0f;
        DensityCurrent[DTid] = 0.0f;
        ReactionCurrent[DTid] = 0.0f;
    }
}