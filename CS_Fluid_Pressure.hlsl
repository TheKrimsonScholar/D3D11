Texture3D<float> PressurePrevious : register(t0);
Texture3D<float> DensityPrevious : register(t1);
Texture3D<float> Divergence : register(t2);

RWTexture3D<float> PressureCurrent : register(u0);

bool IsCellInBounds(uint3 coordinates)
{
    return true;
    return coordinates.x >= 0 && coordinates.x < 256 
        && coordinates.y >= 0 && coordinates.y < 256 
        && coordinates.z >= 0 && coordinates.z < 256;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{   
    float centerPressure = PressurePrevious[DTid];
    
    float pressureRight = IsCellInBounds(DTid + uint3(1, 0, 0)) ? PressurePrevious[DTid + uint3(1, 0, 0)] : centerPressure;
    float pressureLeft = IsCellInBounds(DTid + uint3(-1, 0, 0)) ? PressurePrevious[DTid + uint3(-1, 0, 0)] : centerPressure;
    float pressureUp = IsCellInBounds(DTid + uint3(0, 1, 0)) ? PressurePrevious[DTid + uint3(0, 1, 0)] : centerPressure;
    float pressureDown = IsCellInBounds(DTid + uint3(0, -1, 0)) ? PressurePrevious[DTid + uint3(0, -1, 0)] : centerPressure;
    float pressureForward = IsCellInBounds(DTid + uint3(0, 0, 1)) ? PressurePrevious[DTid + uint3(0, 0, 1)] : centerPressure;
    float pressureBackward = IsCellInBounds(DTid + uint3(0, 0, -1)) ? PressurePrevious[DTid + uint3(0, 0, -1)] : centerPressure;
    
    float divergence = Divergence[DTid];
    
    float pressure = (
        pressureLeft + pressureRight 
        + pressureDown + pressureUp 
        + pressureBackward + pressureForward - divergence) / 6.0f;

    PressureCurrent[DTid] = pressure;
}