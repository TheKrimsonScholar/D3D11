#ifndef __SHADER_FUNCTIONS__
#define __SHADER_FUNCTIONS__

#include "ShaderStructs.hlsli"

float attenuate(Light light, float3 worldPosition)
{
    float dist = distance(light.Location, worldPosition);
    float attenuated = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    
    return attenuated * attenuated;
}

float3 diffuse(VertexToPixel input, Light light)
{
    // Direction is represented differently based on light type
    float3 lightDirection;
    switch(light.LightType)
    {
        case LIGHT_TYPE_DIRECTIONAL:
            lightDirection = light.Direction;
            break;
        case LIGHT_TYPE_POINT:
        case LIGHT_TYPE_SPOT:
            lightDirection = normalize(input.worldPosition - light.Location);
            break;
    }
    lightDirection = normalize(lightDirection);

    float3 surfaceToLight = -normalize(lightDirection);
    float3 diffuseColor = saturate(dot(input.normal, surfaceToLight)) * light.Color * light.Intensity;
    
    if(light.LightType == LIGHT_TYPE_POINT || light.LightType == LIGHT_TYPE_SPOT)
        diffuseColor *= attenuate(light, input.worldPosition);
    
    return diffuseColor;
}
float3 specular(VertexToPixel input, Light light, float3 cameraLocation, float roughness)
{
    // Direction is represented differently based on light type
    float3 lightDirection;
    switch(light.LightType)
    {
        case LIGHT_TYPE_DIRECTIONAL:
            lightDirection = light.Direction;
            break;
        case LIGHT_TYPE_POINT:
        case LIGHT_TYPE_SPOT:
            lightDirection = normalize(input.worldPosition - light.Location);
            break;
    }
    lightDirection = normalize(lightDirection);

    float specularExp = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    if(specularExp > 0.05f)
    {
        float3 view = normalize(cameraLocation - input.worldPosition);
        float3 reflection = reflect(lightDirection, input.normal);

        return pow(saturate(dot(reflection, view)), specularExp);
    }
    return 0;
}

#endif