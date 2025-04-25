struct Particle
{
    bool isAlive;
    float age;
    float lifetime;
    float3 location;
    float rotation;
    float3 velocity;
    float3 acceleration;
    float4 color;
};

struct VertexToPixel_Particle
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

// RNG =====================
// Random number generation
// From: https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/
// Be sure to initialize rng_state first!
static const float uint2float = 1.0 / 4294967296.0;

uint rand_pcg(inout uint rng_state)
{
    uint state = rng_state;
    rng_state = rng_state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float rand_float(inout uint rng_state)
{
    return rand_pcg(rng_state) * uint2float;
}

float rand_float(inout uint rng_state, float min, float max)
{
    return rand_float(rng_state) * (max - min) + min;
}