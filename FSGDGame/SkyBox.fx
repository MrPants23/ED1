#ifndef SKY_BOX_FX
#define SKY_BOX_FX

#include "CommonDeclarations.fx"

/// Vertex output for cubemaps
struct CubeVertOut
{
	float4 position		: POSITION0;
	float3 texCoord		: TEXCOORD0;
};

/// A vertex shader used to plot the positions of a skybox.  The positions should define 
/// a unit cube about the origin.  They will be used as texture coordinates and ofset by the camera.
/// position - The position of the vertex.
/// \return Returns a filled out CubeVertOut for this vertex
CubeVertOut SkyBoxVS(float3 position : POSITION0)
{
	CubeVertOut outVert;
	outVert.texCoord = float3(position.x, position.y, position.z);
	float3 pos = position + gCameraPos;
	outVert.position = mul(float4(pos, 1.0), gMVP);
	outVert.position.z = outVert.position.w;

	return outVert;
}

/// A simple fragment shader used to find the color in a cubemap texture
float4 SkyBoxPS(CubeVertOut inVert) : COLOR 
{
	return texCUBE(reflectionSampler, inVert.texCoord);
}

technique Basic
{
	pass P0
	{
	    vertexShader = compile vs_2_0 SkyBoxVS();
        pixelShader  = compile ps_2_0 SkyBoxPS();

        CullMode = CCW;
		ZWriteEnable = FALSE;

	}
}

#endif