Texture3D<float3> VelocityPrevious : register(t0);
Texture3D<float3> PressurePrevious : register(t1);

RWTexture3D<float3> VelocityCurrent : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 pressureRight = PressurePrevious[DTid + uint3(1, 0, 0)];
    float3 pressureLeft = PressurePrevious[DTid + uint3(-1, 0, 0)];
    float3 pressureUp = PressurePrevious[DTid + uint3(0, 1, 0)];
    float3 pressureDown = PressurePrevious[DTid + uint3(0, -1, 0)];
    float3 pressureForward = PressurePrevious[DTid + uint3(0, 0, 1)];
    float3 pressureBackward = PressurePrevious[DTid + uint3(0, 0, -1)];
    
    float3 pressureGradient = 0.5f * float3(
        length(pressureRight - pressureLeft),
        length(pressureUp - pressureDown),
        length(pressureForward - pressureBackward));
    float3 velocityOld = VelocityPrevious[DTid].xyz;
    float3 velocityNew = velocityOld - pressureGradient;
    
    VelocityCurrent[DTid] = velocityNew;
}