cbuffer ExternalData : register(b0)
{
    float deltaTime;
};

Texture3D<float3> VelocityPrevious : register(t0);
Texture3D<float3> PressurePrevious : register(t1);
Texture3D<float3> DensityPrevious : register(t2);

SamplerState Sampler : register(s0);

RWTexture3D<float3> VelocityCurrent : register(u0);
RWTexture3D<float3> PressureCurrent : register(u1);
RWTexture3D<float3> DensityCurrent : register(u2);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 velocity = VelocityPrevious[DTid];
    float3 X = DTid - velocity * deltaTime;
    
    /* Velocity */
    {
        float3 Q = VelocityPrevious.SampleLevel(Sampler, X, 0);
        VelocityCurrent[DTid] = Q;
    }
    /* Pressure */
    {
        float3 Q = PressurePrevious.SampleLevel(Sampler, X, 0);
        PressureCurrent[DTid] = Q;
    }
    /* Density */
    {
        float3 Q = DensityPrevious.SampleLevel(Sampler, X, 0);
        DensityCurrent[DTid] = Q;
    }
}