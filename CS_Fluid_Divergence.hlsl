Texture3D<float3> VelocityPrevious : register(t0);

RWTexture3D<float> Divergence : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 velocityRight = VelocityPrevious[DTid + uint3(1, 0, 0)];
    float3 velocityLeft = VelocityPrevious[DTid + uint3(-1, 0, 0)];
    float3 velocityUp = VelocityPrevious[DTid + uint3(0, 1, 0)];
    float3 velocityDown = VelocityPrevious[DTid + uint3(0, -1, 0)];
    float3 velocityForward = VelocityPrevious[DTid + uint3(0, 0, 1)];
    float3 velocityBackward = VelocityPrevious[DTid + uint3(0, 0, -1)];
    
    float divergence = 0.5f * (velocityRight - velocityLeft) + (velocityUp - velocityDown) + (velocityForward - velocityBackward);
    
    Divergence[DTid] = divergence;
}