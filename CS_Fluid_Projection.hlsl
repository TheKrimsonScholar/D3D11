Texture3D<float4> VelocityPrevious : register(t0);
Texture3D<float> PressurePrevious : register(t1);
Texture3D<float> DensityPrevious : register(t2);

RWTexture3D<float4> VelocityCurrent : register(u0);

bool IsCellInBounds(uint3 coordinates)
{
    //return true;
    return coordinates.x >= 0 && coordinates.x < 256
        && coordinates.y >= 0 && coordinates.y < 256
        && coordinates.z >= 0 && coordinates.z < 256;
}

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
    
    //if(!IsCellInBounds(DTid + uint3(1, 0, 0)))
    //    velocityNew.x = min(velocityNew.x, 0);
    //if(!IsCellInBounds(DTid + uint3(-1, 0, 0)))
    //    velocityNew.x = max(velocityNew.x, 0);
    
    //if(!IsCellInBounds(DTid + uint3(0, 1, 0)))
    //    velocityNew.y = min(velocityNew.y, 0);
    //if(!IsCellInBounds(DTid + uint3(0, -1, 0)))
    //    velocityNew.y = max(velocityNew.y, 0);
    
    //if(!IsCellInBounds(DTid + uint3(0, 0, 1)))
    //    velocityNew.z = min(velocityNew.z, 0);
    //if(!IsCellInBounds(DTid + uint3(0, 0, -1)))
    //    velocityNew.z = max(velocityNew.z, 0);
    
    VelocityCurrent[DTid] = float4(velocityNew, 1);
}