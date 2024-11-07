struct VertexToPixel_Sky
{
	float4 position				: SV_POSITION;
	float4 sampleDirection		: DIRECTION;
};

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}