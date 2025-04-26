Texture3D<float3> PressurePrevious : register(t0);
Texture3D<float> Divergence : register(t1);

RWTexture3D<float3> PressureCurrent : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 pressureRight = PressurePrevious[DTid + uint3(1, 0, 0)];
    float3 pressureLeft = PressurePrevious[DTid + uint3(-1, 0, 0)];
    float3 pressureUp = PressurePrevious[DTid + uint3(0, 1, 0)];
    float3 pressureDown = PressurePrevious[DTid + uint3(0, -1, 0)];
    float3 pressureForward = PressurePrevious[DTid + uint3(0, 0, 1)];
    float3 pressureBackward = PressurePrevious[DTid + uint3(0, 0, -1)];
    
    float divergence = Divergence[DTid];
    
    float pressure = (
        pressureLeft + pressureRight 
        + pressureDown + pressureUp 
        + pressureBackward + pressureForward - divergence) / 6.0f;

    PressureCurrent[DTid] = pressure;
}