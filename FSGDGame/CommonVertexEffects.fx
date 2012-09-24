#include "LightingSupport.fx"

#ifndef COMMON_VERTEX_EFFECTS_FX
#define COMMON_VERTEX_EFFECTS_FX

/// A common vertex shader used by most techniques.  This vertex shader will transform the position and normal
/// as well as fill out the UV and all light properties.
/// \param position - The position of the vertex
/// \param normal - The normal of the vertex
/// \param texCoord - The UV of the vertex
/// \return Returns a filled out BaseVertexOut, based on the current vertex.
BaseVertexOut LitTexturedVS(float3 position : POSITION0, float3 normal : NORMAL, float2 texCoord : TEXCOORD0)
{
	BaseVertexOut outVert;

	// Transfrom the position
	outVert.position = mul(float4(position, 1.0), gMVP);
	outVert.screenPos = outVert.position;
	float4 worldPos = mul(float4(position, 1.0), gWorld);

	outVert.spotLightPos = mul(worldPos, SpotLight0.ViewProjection);

	// Transform the normal, without translation
	outVert.normal = mul(float4(normal, 0.0), gWorld);

	// Pass the texCoord
	outVert.texCoord = texCoord;

	// Calculate vertex components of lighting
	outVert.pointDirDist = ApplyPointLightVS(PointLight0, worldPos);
	outVert.spotDirDist = ApplySpotLightVS(SpotLight0, worldPos);

	float3 dirFromCam = normalize(worldPos.xyz - gCameraPos);
	outVert.pointHalfVec = normalize(outVert.pointDirDist.xyz + dirFromCam);
	outVert.spotHalfVec = normalize(outVert.spotDirDist.xyz + dirFromCam);
	outVert.dirHalfVec = normalize(DirLight.Direction.xyz + dirFromCam);

	return outVert;
}

/// A common vertex shader used by most techniques.  This vertex shader will transform the position and normal
/// as well as fill out the UV and all light properties.
/// \param position - The position of the vertex
/// \param normal - The normal of the vertex
/// \param texCoord - The UV of the vertex
/// \return Returns a filled out BaseVertexOut, based on the current vertex.
BaseVertexOut LitTexturedBumpVS(float3 position : POSITION0, float3 normal : NORMAL, float3 tangent : TANGENT, float2 texCoord : TEXCOORD0)
{
	BaseVertexOut outVert;

	// Transfrom the position
	outVert.position = float4(0, 0, 0, 0);//mul(float4(position, 1.0), gMVP);
	outVert.screenPos = outVert.position;
	float4 worldPos = mul(float4(position, 1.0), gWorld);

	outVert.spotLightPos = mul(worldPos, SpotLight0.ViewProjection);

	// Transform the normal, without translation
	outVert.normal = mul(float4(normal, 0.0), gWorld);

	// Pass the texCoord
	outVert.texCoord = texCoord;

	// Calculate vertex components of lighting
	outVert.pointDirDist = ApplyPointLightVS(PointLight0, worldPos);
	outVert.spotDirDist = ApplySpotLightVS(SpotLight0, worldPos);

	float3 dirFromCam = normalize(worldPos.xyz - gCameraPos);
	outVert.pointHalfVec = normalize(outVert.pointDirDist.xyz + dirFromCam);
	outVert.spotHalfVec = normalize(outVert.spotDirDist.xyz + dirFromCam);
	outVert.dirHalfVec = normalize(DirLight.Direction.xyz + dirFromCam);

	return outVert;
}

#endif