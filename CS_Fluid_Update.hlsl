cbuffer ExternalData : register(b0)
{
    float deltaTime;
};

Texture3D<float4> VelocityPrevious : register(t0);
Texture3D<float> TemperaturePrevious : register(t1);

RWTexture3D<float4> VelocityCurrent : register(u0);
RWTexture3D<float> TemperatureCurrent : register(u1);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    VelocityCurrent[DTid] = VelocityPrevious[DTid];
    
    float3 uvw = DTid / float3(256.0f, 256.0f, 256.0f);
    float3 diff = uvw - float3(0.5f, 0.5f, 0.5f);
    if(length(diff) < 0.1f)
        TemperatureCurrent[DTid] = 1.0f;
}