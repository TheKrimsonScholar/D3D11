/* Liquids */

// Maximum signed distance from liquid surface allowed for a cell to be considered within the liquid
static const float LEVEL_SET_THRESHOLD = 0.1f;

struct VertexToPixel_Fluid
{
    float4 screenPosition : SV_POSITION;
    float4 worldPosition : POSITION;
};