//#define D3DXSHADER_DEBUG
//#define D3DXSHADER_SKIPOPTIMIZATION
#include "CommonDeclarations.fx"

#ifndef LIGHTING_SUPPORT_FX
#define LIGHTING_SUPPORT_FX

/// A method used to compute the amount of specular highlighting a fragment should receive.
/// \param unitHalfVec - The normalized half vector between the light and camera
/// \param normal - The normal of the fragment
/// \param intensity - The intensity of the specular reflection desired
/// \param power - The power of the  specular reflection desired
/// \return Retursn the amount of specular color that should be used.
float ComputeBlinnSpecularModulation(float3 unitHalfVec, float3 normal, float intensity, float power)
{
    float coeff = saturate(dot(unitHalfVec, normal));
    return intensity * pow(coeff, power);
}

/// A method used to find the color after lighting from a directional light.
/// \param dirLight - The DirectionalLight to light this surface
/// \param halfVec - A vector half way between the light and camera
/// \param normal - The normal of the surface to light
/// \param matColor - The color of the surface before lighting
float4 ApplyDirectionalLight(DirectionalLight dirLight, float3 halfVec, float4 normal, float4 matColor)
{
	float4 diffuse, ambient, specular, color;
	float nDotL, specMod;

	color = float4(0, 0, 0, 0);
    if(dirLight.Enabled)
	{
		nDotL = dot(-dirLight.Direction, normal.xyz);
		diffuse = matColor * dirLight.DiffuseColor;
		specular = matColor * dirLight.SpecularColor;
		ambient = matColor * dirLight.AmbientColor;

		specMod = ComputeBlinnSpecularModulation(normalize(halfVec), normal.xyz, 
			dirLight.SpecularIntensity, dirLight.SpecularPower);
		color = (nDotL * diffuse + ambient + specMod * specular);
	}
	return color;
}

/// A method used to find the distance and direction to a point light
/// \param pointLight - The PointLight to use
/// \param position - The position on the surface to use
/// \return Returns a float4, the first 3 components are the normalized light direction, 
/// the fourth is the distance to the light
float4 ApplyPointLightVS(PointLight pointLight, float4 position)
{
	float4 dirDist = float4(0, 0, 0, 0);

    if(pointLight.Enabled)
	{
		dirDist.xyz = pointLight.Position - position.xyz;
		dirDist.w = length(dirDist.xyz);
	}
    return dirDist;
}

/// A method used to find the distance and direction to a spot light
/// \param spotLight - The SpotLight to use
/// \param position - The position on the surface to use
/// \return Returns a float4, the first 3 components are the normalized light direction, 
/// the fourth is the distance to the light
float4 ApplySpotLightVS(SpotLight spotLight, float4 position)
{
	float dist;
	float4 dirDist = float4(0, 0, 0, 0);
    if(spotLight.Enabled)
	{
		dirDist = spotLight.Position - position;
		dist = length(dirDist);
		dirDist.w = dist;
	}
    return dirDist;
}

/// A method used to find the color after lighting from a point light.
/// \param pointLight - The PointLight to light this surface
/// \param halfVec - A vector half way between the light and camera
/// \param dirDistance - x,y,z should be the normalized light direction, 
/// w should be distance to the light
/// \param normal - The normal of the surface to light
/// \param ambientMatColor - The ambient color of the surface material
/// \param diffuseMatColor - The diffuse color of the surface material
/// \param specularMatColor - The specular color of the surface material
/// \return Returns the resulting color of the surface after applying the 
/// point light
float4 ApplyPointLightPS(PointLight pointLight, float3 halfVec, 
	float4 dirDistance, float4 normal, float4 ambientMatColor, 
	float4 diffuseMatColor, float4 specularMatColor)
{
	float4 diffuse, ambient, specular, color;
	float dist, nDotL, attenuation, specMod;

	color = float4(0, 0, 0, 0);

    if(pointLight.Enabled)
	{
		diffuse = diffuseMatColor * pointLight.DiffuseColor;
		ambient = ambientMatColor * pointLight.AmbientColor;
		specular = specularMatColor * pointLight.SpecularColor;

		nDotL = dot(normalize(dirDistance.xyz), normal.xyz);
		
		if(nDotL > 0.0)
		{
			attenuation = saturate(1.0 / (pointLight.Attenuation.x +
					pointLight.Attenuation.y * dirDistance.w +
					pointLight.Attenuation.z * dirDistance.w * dirDistance.w));
			// Specular 
			specMod = ComputeBlinnSpecularModulation(normalize(halfVec), normal.xyz, 
				pointLight.SpecularIntensity, pointLight.SpecularPower);

			color = attenuation * (nDotL * diffuse + ambient + specMod * specular);
		}
	}

	return color;
}

/// A method used to find the color after lighting from a point light, with 
/// shadow mapping.
/// \param pointLight - The PointLight to light this surface
/// \param halfVec - A vector half way between the light and camera
/// \param dirDistance - x,y,z should be the normalized light direction, 
/// w should be distance to the light
/// \param normal - The normal of the surface to light
/// \param ambientMatColor - The ambient color of the surface material
/// \param diffuseMatColor - The diffuse color of the surface material
/// \param specularMatColor - The specular color of the surface material
/// \return Returns the resulting color of the surface after applying the 
/// point light
float4 ApplyPointLightWithShadowsPS(PointLight pointLight, float3 halfVec, 
	float4 dirDistance, float4 normal, float4 ambientMatColor, 
	float4 diffuseMatColor, float4 specularMatColor)
{
	float pointDepth, depth;
	float4 pointColor;
	float3 texPointCoords;
	
	texPointCoords = -(dirDistance.xyz);
	pointDepth = texCUBE(point0ShadowSampler, texPointCoords).x;
	
	depth = max(max(abs(dirDistance.x), abs(dirDistance.y)), abs(dirDistance.z));
	if((depth / dirDistance.w) > pointDepth)
		pointColor = float4(0, 0, 0, 0);
	else
		pointColor = ApplyPointLightPS(PointLight0, halfVec, dirDistance, 
			normal, ambientMatColor, diffuseMatColor, specularMatColor);
	return pointColor;
}

/// A method used to find the color after lighting from a spot light.
/// \param spotLight - The SpotLight to light this surface
/// \param halfVec - A vector half way between the light and camera
/// \param dirDistance - x,y,z should be the normalized light direction, w should be distance to the light
/// \param normal - The normal of the surface to light
/// \param matColor - The color of the surface before lighting
/// \return Returns the resulting color of the surface after applying the spot light
float4 ApplySpotLightPS(SpotLight spotLight, float3 halfVec, float4 dirDistance,
	 float4 normal, float4 matColor)
{
	float4 diffuse, ambient, specular, color;
	float dist, nDotL, attenuation, spotEffect, specMod;

	color = float4(0, 0, 0, 0);
    if(spotLight.Enabled)
	{
		// Initialize color values
        diffuse = matColor * spotLight.DiffuseColor;
        ambient = matColor * spotLight.AmbientColor;
        specular = matColor * spotLight.AmbientColor;

        dist = dirDistance.w;
        dirDistance.xyz = normalize(dirDistance.xyz);
        nDotL = dot(dirDistance.xyz, normal.xyz);
        if(nDotL > 0.0)
		{
            spotEffect = dot(normalize(spotLight.Direction.xyz), -dirDistance.xyz);
            if( spotEffect > spotLight.Cutoff)
			{
                spotEffect = pow(abs(spotEffect), abs(spotLight.Exponent));
                attenuation = spotEffect / (spotLight.Attenuation.x +
										spotLight.Attenuation.y * dist +
										spotLight.Attenuation.z * dist * dist);
                // Specular
				specMod = ComputeBlinnSpecularModulation(normalize(halfVec), normal.xyz, 
					spotLight.SpecularIntensity, spotLight.SpecularPower);
					color += attenuation * (nDotL * diffuse + specMod * specular);
            }
		}
	}
	return color;
}

/// A method used to find the color after lighting from a spot light, with 
/// shadow mapping.
/// \param spotLight - The SpotLight to light this surface
/// \param halfVec - A vector half way between the light and camera
/// \param posInLightSpace - the position of the fragment to be lit in light 
/// space
/// \param dirDistance - x,y,z should be the normalized light direction, 
/// w should be distance to the light
/// \param normal - The normal of the surface to light
/// \param matColor - The color of the surface before lighting
/// \return Returns the resulting color of the surface after applying the spot light
float4 ApplySpotLightWithShadowsPS(SpotLight spotLight, float3 halfVec, 
	float4 posInLightSpace, float4 dirDistance, float4 normal, float4 matColor)
{
	float4 spotColor;
	float3 lightPosPostW;
	float2 texCoords;
	float lightDepth;

	// Spot shadows
	lightPosPostW = posInLightSpace.xyz / posInLightSpace.w;
	texCoords = 0.5 * lightPosPostW.xy + float2( 0.5, 0.5 );
	texCoords.y = 1.0f - texCoords.y;
	lightDepth = tex2D(spot0ShadowSampler, texCoords).r;
	if(lightDepth < lightPosPostW.z)
		spotColor = float4(0, 0, 0, 0);
	else 
		spotColor = ApplySpotLightPS(SpotLight0, halfVec, dirDistance, 
			normal, matColor);
	return spotColor;
}

#endif