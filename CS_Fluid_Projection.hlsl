Texture3D<float4> VelocityPrevious : register(t0);
Texture3D<float> PressurePrevious : register(t1);
Texture3D<float> DensityPrevious : register(t2);

RWTexture3D<float4> VelocityCurrent : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float pressureRight = PressurePrevious[DTid + uint3(1, 0, 0)];
    float pressureLeft = PressurePrevious[DTid + uint3(-1, 0, 0)];
    float pressureUp = PressurePrevious[DTid + uint3(0, 1, 0)];
    float pressureDown = PressurePrevious[DTid + uint3(0, -1, 0)];
    float pressureForward = PressurePrevious[DTid + uint3(0, 0, 1)];
    float pressureBackward = PressurePrevious[DTid + uint3(0, 0, -1)];
    
    float3 pressureGradient = 0.5f * float3(
        pressureRight - pressureLeft,
        pressureUp - pressureDown,
        pressureForward - pressureBackward);
    float3 velocityOld = VelocityPrevious[DTid].xyz;
    float3 velocityNew = velocityOld - pressureGradient;
    
    VelocityCurrent[DTid] = float4(velocityNew, 1);
}