Texture3D<float> PressurePrevious : register(t0);
Texture3D<float> Divergence : register(t1);

RWTexture3D<float> PressureCurrent : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float pressureRight = PressurePrevious[DTid + uint3(1, 0, 0)];
    float pressureLeft = PressurePrevious[DTid + uint3(-1, 0, 0)];
    float pressureUp = PressurePrevious[DTid + uint3(0, 1, 0)];
    float pressureDown = PressurePrevious[DTid + uint3(0, -1, 0)];
    float pressureForward = PressurePrevious[DTid + uint3(0, 0, 1)];
    float pressureBackward = PressurePrevious[DTid + uint3(0, 0, -1)];
    
    float divergence = Divergence[DTid];
    
    float pressure = (
        pressureLeft + pressureRight 
        + pressureDown + pressureUp 
        + pressureBackward + pressureForward - divergence) / 6.0f;

    PressureCurrent[DTid] = pressure;
}