#include "Particles.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTintMin;
    float4 colorTintMax;
    float3 locationMin;
    float rotationMin;
    float3 locationMax;
    float rotationMax;
    float lifetimeMin;
    float3 velocityMin;
    float lifetimeMax;
    float3 velocityMax;
    float3 accelerationMin;
    uint emitCount;
    float3 accelerationMax;
}
cbuffer DeadListCounter : register(b1)
{
    uint deadListCounter;
}

RWStructuredBuffer<Particle> ParticlePool : register(u0);
ConsumeStructuredBuffer<uint> DeadList : register(u1);

[numthreads(32, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if(DTid.x >= (uint) emitCount || deadListCounter == 0 || DTid.x >= deadListCounter)
        return;
    
    uint index = DeadList.Consume();
    Particle particle = ParticlePool.Load(index);
    
    uint rng_state = index;
    
    // Update particle data
    particle.isAlive = true;
    particle.age = 0;
    particle.color = float4(
        rand_float(rng_state, colorTintMin.r, colorTintMax.r), 
        rand_float(rng_state, colorTintMin.g, colorTintMax.g), 
        rand_float(rng_state, colorTintMin.b, colorTintMax.b), 
        rand_float(rng_state, colorTintMin.a, colorTintMax.a));
    particle.lifetime = rand_float(rng_state, lifetimeMax, lifetimeMax);
    particle.location = float3(
        rand_float(rng_state, locationMin.x, locationMax.x),
        rand_float(rng_state, locationMin.y, locationMax.y),
        rand_float(rng_state, locationMin.z, locationMax.z));
    particle.rotation = rand_float(rng_state, rotationMax, rotationMax);
    particle.velocity = float3(
        rand_float(rng_state, velocityMin.x, velocityMax.x), 
        rand_float(rng_state, velocityMin.y, velocityMax.y), 
        rand_float(rng_state, velocityMin.z, velocityMax.z));
    particle.acceleration = float3(
        rand_float(rng_state, accelerationMin.x, accelerationMax.x),
        rand_float(rng_state, accelerationMin.y, accelerationMax.y),
        rand_float(rng_state, accelerationMin.z, accelerationMax.z));
    
    ParticlePool[index] = particle;
}