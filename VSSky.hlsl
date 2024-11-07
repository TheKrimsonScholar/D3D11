#include "ShaderStructs.hlsli"

cbuffer DataFromCPU : register(b0)
{
	matrix worldMatrix;
	matrix projectionMatrix;
};

struct VertexToPixel_Sky
{
	float4 position				: SV_POSITION;
	float4 sampleDirection		: DIRECTION;
};

VertexToPixel_Sky main(VertexShaderInput input)
{
	VertexToPixel_Sky output;

	matrix viewNoTranslation = viewMatrix;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	output.position = mul(viewNoTranslation, mul(projectionMatrix, input.position));
	output.position.z = output.position.w;

	output.sampleDirection = float4(1, 0, 0, 1);

	return output;
}