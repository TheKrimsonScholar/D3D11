#include "Fluids.hlsli"

cbuffer ExternalData : register(b0)
{   
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projMatrix;
};

VertexToPixel_Fluid main(uint id : SV_VertexID)
{
    /* Manually draw a box centered at the world position */
    
    static const float3 BOX_VERTICES[8] =
    {
        float3(-1, -1, -1), // Back bottom left
        float3(1, -1, -1),  // Back bottom right
        float3(-1, 1, -1),  // Back top left
        float3(1, 1, -1),   // Back top right
        float3(-1, -1, 1),  // Front bottom left
        float3(1, -1, 1),   // Front bottom right
        float3(-1, 1, 1),   // Front top left
        float3(1, 1, 1)     // Front top right
    };
    static const uint BOX_INDICES[36] =
    {
        // Back face
        0, 2, 1,
        2, 3, 1,
        
        // Front face
        4, 5, 6,
        6, 5, 7,
        
        // Left face
        0, 4, 2,
        4, 6, 2,
        
        // Right face
        1, 3, 5,
        5, 3, 7,
        
        // Bottom face
        0, 1, 4,
        4, 1, 5,
        
        // Top face
        2, 6, 3,
        6, 7, 3
    };
    
    float3 localPosition = BOX_VERTICES[BOX_INDICES[id]];
    
    float4 worldPosition = mul(worldMatrix, float4(localPosition, 1));
    float4 screenPosition = mul(projMatrix, mul(viewMatrix, worldPosition));
    
    VertexToPixel_Fluid output;
    output.screenPosition = screenPosition;
    output.worldPosition = worldPosition;
    
    return output;
}