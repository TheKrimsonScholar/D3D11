cbuffer ExternalData : register(b0)
{
    uint verticesPerParticle;
}

RWBuffer<uint> DrawArgs : register(u0);
RWStructuredBuffer<uint> DrawList : register(u1);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    DrawArgs[0] = DrawList.IncrementCounter() * verticesPerParticle; // IndexCountPerInstance
    DrawArgs[1] = 1; // InstanceCount
    DrawArgs[2] = 0; // Offset
    DrawArgs[3] = 0; // Offset
    DrawArgs[4] = 0; // Offset
}