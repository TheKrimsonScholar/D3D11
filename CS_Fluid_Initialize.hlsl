RWTexture3D<float4> VelocityCurrent : register(u0);
RWTexture3D<float> PressureCurrent : register(u1);
RWTexture3D<float> DensityCurrent : register(u2);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 uvw = DTid / float3(256.0f, 256.0f, 256.0f);
    float3 diff = uvw - float3(0.5f, 0.5f, 0.5f);
    if(length(diff) < 0.5f)
    {
        VelocityCurrent[DTid] = float4(0.1f, 0, 0, 0);
        PressureCurrent[DTid] = 0.001f;
        DensityCurrent[DTid] = 1.0f;
    }
    else
    {
        VelocityCurrent[DTid] = float4(0, 0, 0, 0);
        PressureCurrent[DTid] = 0.0f;
        DensityCurrent[DTid] = 0.0f;
    }
}