#include "CommonDeclarations.fx"

#ifndef G_BUFFERS_BUMP_FX
#define G_BUFFERS_BUMP_FX

struct GBufferBumpVertexOut
{
	float4 position		: POSITION0;
	float2 texCoord		: TEXCOORD0;
	float3x3 tbn		: TEXCOORD1;
};

GBufferBumpVertexOut GBuffersBumpVS(float3 position : POSITION0, 
	float3 normal : NORMAL, float3 tangent : TANGENT, 
	float2 texCoord : TEXCOORD0)
{

}

GBufferFragOut GBuffersBumpFS(GBufferBumpVertexOut inVert)
{

}

/// A vertex shader used to plot depths of a given vertex.  
/// \param position - The position of the vertex
/// \param normal - The normal of the vertex, ignored
/// \param texCoord - The UV of the vertex, ignored
/// \return Returns a DepthVertOut based on the vertex
DepthVertOut DepthCamBumpVertex(float3 position : POSITION0, 
	float3 normal : NORMAL, float3 tangent : TANGENT, 
	float2 texCoord : TEXCOORD0)
{

}

/// A fragment shader used to write normalized depth values as colors
float4 DepthCamBumpFragment( DepthVertOut inVert) : COLOR
{

}

technique GBuffersBump
{
	pass P0
	{
	    vertexShader = compile vs_2_0 GBuffersBumpVS();
        pixelShader  = compile ps_2_0 GBuffersBumpFS();

        CullMode = CCW;
		ZWriteEnable = FALSE;
	}
}

technique DepthCameraBump
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthCamBumpVertex();
        pixelShader  = compile ps_2_0 DepthCamBumpFragment();

        CullMode = CCW;
    }
}


#endif
