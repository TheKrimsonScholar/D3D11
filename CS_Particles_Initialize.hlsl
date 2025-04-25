cbuffer ExternalData : register(b0)
{
    uint maxParticles;
}

AppendStructuredBuffer<uint> DeadList : register(u0);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if(DTid.x >= (uint) maxParticles)
        return;
    
    DeadList.Append(DTid.x);
}