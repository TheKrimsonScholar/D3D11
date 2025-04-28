cbuffer ExternalData : register(b0)
{
    float deltaTime;
    float coolingRate;
};

Texture3D<float> TemperaturePrevious : register(t0);

RWTexture3D<float> TemperatureCurrent : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    TemperatureCurrent[DTid] = TemperaturePrevious[DTid] * exp(-coolingRate * deltaTime);
}