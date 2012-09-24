//#define D3DXSHADER_DEBUG
//#define D3DXSHADER_SKIPOPTIMIZATION

#include "CommonVertexEffects.fx"
#include "Depth.fx"
#include "GBuffers.fx"
#include "GBuffersBump.fx"

#ifndef TERRAIN_FX
#define TERRAIN_FX

/// The diffuse texture sampler used for the main ground texture
sampler groundSampler = 
sampler_state
{
	Texture = <diffuseMap>;
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

/// The diffuse texture sampler used for the secondary ground texture
sampler dirtSampler = 
sampler_state
{
	Texture = <specularMap>;
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

/// The diffuse texture sampler used for the noise ground texture
sampler noiseSampler = 
sampler_state
{
	Texture = <normalMap>;
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

/// A fragment shader used to color a surface that represents terrain.  Multitexturing and a point, spot and directional light
/// will be used to define the color of the fragment.
/// \param inVert - The interpolated vertex that defines this fragment.
/// \return Returns the color of the fragment.
float4 TerrainPS(BaseVertexOut inVert) : COLOR
{
	float4 normal, dirColor, pointColor, spotColor, vTexColor0, vTexColor1, 
		vTexColor2, vTexColor3, vTexColor;
	float biasY, offset, compA, compB, rem, value, ratio, scalar;

	// Sample the three textures
	vTexColor0 = tex2D(groundSampler, inVert.texCoord);
	vTexColor1 = tex2D(dirtSampler, inVert.texCoord);
	vTexColor2 = tex2D(noiseSampler, inVert.texCoord);

	vTexColor3 = float4(0, 0, 0, 0);
	vTexColor  = float4(0, 0, 0, 0);

	biasY = min( 1.0, inVert.normal.y + 0.025);

	offset = pow( abs(biasY), 8.0 );

	vTexColor += vTexColor0 * offset * offset;
	vTexColor += vTexColor1 * (1.0 - offset)*(1.0 - offset);

	compA = offset*offset;
	compB = (1.0 - offset)*(1.0 - offset);
	rem = max( 0.0, 1.0 - (compA + compB) );

	value = min( 1.0, vTexColor2.x + 0.3 );

	ratio = compA / (compA + compB);
	scalar = rem * ratio * value;
	vTexColor3 += vTexColor0 * scalar;
	scalar = rem * (1.0 - ratio ) * (1.0 - value);
	vTexColor3 += vTexColor1 * scalar;
	vTexColor += vTexColor3;

	normal = normalize(inVert.normal);

	dirColor = ApplyDirectionalLight(DirLight, inVert.dirHalfVec, normal, vTexColor);

	// Point light and shadows
	pointColor = ApplyPointLightWithShadowsPS(PointLight0, inVert.pointHalfVec, inVert.pointDirDist, 
			normal, vTexColor, vTexColor, vTexColor);

	// Spot light and shadows
	spotColor = ApplySpotLightWithShadowsPS(SpotLight0, inVert.spotHalfVec, 
		inVert.spotLightPos, inVert.spotDirDist, normal, vTexColor);

	return dirColor + pointColor + spotColor;
}

GBufferFragOut TerrainGBuffersFS(GBufferVertexOut inVert)
{
	GBufferFragOut fragOut;
	float4 norm, vTexColor0, vTexColor1, vTexColor2, vTexColor3, vTexColor;
	float biasY, offset, compA, compB, rem, value, ratio, scalar;

	norm = normalize(inVert.normal);
	norm = norm * .5 + .5;
	fragOut.normal = norm;

	// Sample the three textures
	vTexColor0 = tex2D(groundSampler, inVert.texCoord);
	vTexColor1 = tex2D(dirtSampler, inVert.texCoord);
	vTexColor2 = tex2D(noiseSampler, inVert.texCoord);

	vTexColor3 = float4(0, 0, 0, 0);
	vTexColor  = float4(0, 0, 0, 0);

	biasY = min( 1.0, inVert.normal.y + 0.025);

	offset = pow( abs(biasY), 8.0 );

	vTexColor += vTexColor0 * offset * offset;
	vTexColor += vTexColor1 * (1.0 - offset)*(1.0 - offset);

	compA = offset*offset;
	compB = (1.0 - offset)*(1.0 - offset);
	rem = max( 0.0, 1.0 - (compA + compB) );

	value = min( 1.0, vTexColor2.x + 0.3 );

	ratio = compA / (compA + compB);
	scalar = rem * ratio * value;
	vTexColor3 += vTexColor0 * scalar;
	scalar = rem * (1.0 - ratio ) * (1.0 - value);
	vTexColor3 += vTexColor1 * scalar;
	vTexColor += vTexColor3;
	fragOut.diffuse = vTexColor;

	fragOut.specular = float4(0, 0, 0, 0);

	return fragOut;
}

technique Terrain
{
    pass P0
    {
        vertexShader = compile vs_3_0 LitTexturedVS();
        pixelShader  = compile ps_3_0 TerrainPS();

        CullMode = CCW;
    }
}

technique TerrainGBuffers
{
	pass P0
	{
	    vertexShader = compile vs_2_0 GBuffersVS();
        pixelShader  = compile ps_2_0 TerrainGBuffersFS();

        CullMode = CCW;
		ZWriteEnable = FALSE;
	}
}
#endif