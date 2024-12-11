cbuffer ExternalData : register(b0)
{
    float2 mouseFocusPoint;
    
    float redOffset;
    float greenOffset;
    float blueOffset;
}

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D Pixels : register(t0);

SamplerState Sampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    /* Chromatic Aberration algorithm from 
    https://lettier.github.io/3d-game-shaders-for-beginners/chromatic-aberration.html */
    
    //return Pixels.Sample(Sampler, input.uv);
    
    float2 textureSize;
    Pixels.GetDimensions(textureSize.x, textureSize.y);
    
    float2 textureCoords = input.position / textureSize;
    
    float2 direction = textureCoords - mouseFocusPoint;
    
    float4 output;
    output.r = Pixels.Sample(Sampler, textureCoords + (direction * float2(redOffset, redOffset))).r;
    output.g = Pixels.Sample(Sampler, textureCoords + (direction * float2(greenOffset, greenOffset))).g;
    output.ba = Pixels.Sample(Sampler, textureCoords + (direction * float2(blueOffset, blueOffset))).b;
    
	return output;
}