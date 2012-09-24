#include "CommonDeclarations.fx"

#ifndef G_BUFFERS_FX
#define G_BUFFERS_FX

struct GBufferVertexOut
{
	float4 position		: POSITION0;
	float4 normal		: TEXCOORD0;
	float2 texCoord		: TEXCOORD1;
};

GBufferVertexOut GBuffersVS(float3 position : POSITION0, 
	float3 normal : NORMAL, float2 texCoord : TEXCOORD0)
{

}

GBufferFragOut GBuffersFS(GBufferVertexOut inVert)
{

}

technique GBuffers
{
	pass P0
	{
	    vertexShader = compile vs_2_0 GBuffersVS();
        pixelShader  = compile ps_2_0 GBuffersFS();

        CullMode = CCW;
		ZWriteEnable = FALSE;
	}
}

/// A vertex shader used to plot depths of a given vertex.  
/// \param position - The position of the vertex
/// \param normal - The normal of the vertex, ignored
/// \param texCoord - The UV of the vertex, ignored
/// \return Returns a DepthVertOut based on the vertex
DepthVertOut DepthCamVertex(float3 position : POSITION0, 
	float3 normal : NORMAL, float2 texCoord : TEXCOORD0)
{

}

/// A fragment shader used to write normalized depth values as colors
float4 DepthCamFragment( DepthVertOut inVert) : COLOR
{

}

technique DepthCamera
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthCamVertex();
        pixelShader  = compile ps_2_0 DepthCamFragment();

        CullMode = CCW;
    }
}

#endif