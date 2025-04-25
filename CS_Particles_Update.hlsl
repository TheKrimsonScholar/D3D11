#include "Particles.hlsli"

cbuffer ExternalData : register(b0)
{
    uint maxParticles;
    float deltaTime;
}

RWStructuredBuffer<Particle> ParticlePool : register(u0);
AppendStructuredBuffer<uint> DeadList : register(u1);
RWStructuredBuffer<uint> DrawList : register(u2);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if(DTid.x >= (uint) maxParticles)
        return;
    
    Particle particle = ParticlePool.Load(DTid.x);
    
    if(!particle.isAlive)
        return;

    particle.age += deltaTime;
    particle.isAlive = particle.age < particle.lifetime;
    
    particle.velocity += particle.acceleration * deltaTime;
    particle.location += particle.velocity * deltaTime;
    
    ParticlePool[DTid.x] = particle;
    
    // If the particle just died, put it at the end of the dead list
    if(!particle.isAlive)
        DeadList.Append(DTid.x);
    else
    {
        uint drawIndex = DrawList.IncrementCounter();
        DrawList[drawIndex] = DTid.x;
    }
}