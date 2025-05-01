cbuffer ExternalData : register(b0)
{
    float deltaTime;
    float coolingRate;
};

Texture3D<float> ReactionPrevious : register(t0);

RWTexture3D<float> ReactionCurrent : register(u0);

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    ReactionCurrent[DTid] = ReactionPrevious[DTid] * exp(-coolingRate * deltaTime);
}