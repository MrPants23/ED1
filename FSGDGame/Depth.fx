#include "LightingSupport.fx"

#ifndef DEPTH_FX
#define DEPTH_FX

/// A vertex shader used to plot depths of a given vertex.  Used in shadow mapping.
/// \param position - The position of the vertex
/// \param normal - The normal of the vertex, ignored
/// \param texCoord - The UV of the vertex, ignored
/// \return Returns a DepthVertOut based on the vertex
DepthVertOut DepthVertexDirectional(float3 position : POSITION0, 
	float3 normal : NORMAL, float2 texCoord : TEXCOORD0)
{
	DepthVertOut outVert;
	
	outVert.position = mul(float4(position, 1.0), gWorld);
	outVert.position = mul(outVert.position, DirLight.ViewProjection);

    outVert.depth.xy = outVert.position.zw;

	return outVert;
}

/// A vertex shader used to plot depths of a given vertex.  Used in shadow mapping.
/// \param position - The position of the vertex
/// \param normal - The normal of the vertex, ignored
/// \param texCoord - The UV of the vertex, ignored
/// \return Returns a DepthVertOut based on the vertex
DepthVertOut DepthVertexSpot(float3 position : POSITION0, 
	float3 normal : NORMAL, float2 texCoord : TEXCOORD0)
{
	DepthVertOut outVert;
	
	outVert.position = mul(float4(position, 1.0), gWorld);
	outVert.position = mul(outVert.position, SpotLight0.ViewProjection);

    outVert.depth.xy = outVert.position.zw;

	return outVert;
}

/// A vertex shader used to plot depths of a given vertex.  Used in shadow 
/// mapping for point lights.
/// \param index - the index of which of the six light projections to use.
/// \param position - The position of the vertex
/// \param normal - The normal of the vertex, ignored
/// \param texCoord - The UV of the vertex, ignored
/// \return Returns a DepthVertOut based on the vertex
DepthVertOut DepthVertexPoint(uniform int index, float3 position : POSITION0,
	float3 normal : NORMAL, float2 texCoord : TEXCOORD0)
{
	DepthVertOut outVert;
	
	outVert.position = mul(float4(position, 1.0), gWorld);
	outVert.position = mul(outVert.position, PointLight0.ViewProjection[index]);

    outVert.depth.xy = outVert.position.zw;

	return outVert;
}

/// A fragment shader used to write normalized depth values as colors
float4 DepthFragment( DepthVertOut inVert) : COLOR
{
    return inVert.depth.x / inVert.depth.y;
}

technique DepthSpotLight
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthVertexSpot();
        pixelShader  = compile ps_2_0 DepthFragment();

        CullMode = CW;
		ZWriteEnable = TRUE;
		ZEnable = TRUE;
    }
}

technique DepthDirectionalLight
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthVertexDirectional();
        pixelShader  = compile ps_2_0 DepthFragment();

        CullMode = CW;
		ZWriteEnable = TRUE;
		ZEnable = TRUE;
    }
}

technique DepthPointLight0
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthVertexPoint(0);
        pixelShader  = compile ps_2_0 DepthFragment();

        CullMode = CCW;
		ZWriteEnable = TRUE;
		ZEnable = TRUE;
    }
}

technique DepthPointLight1
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthVertexPoint(1);
        pixelShader  = compile ps_2_0 DepthFragment();

        CullMode = CCW;
		ZWriteEnable = TRUE;
		ZEnable = TRUE;
    }
}

technique DepthPointLight2
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthVertexPoint(2);
        pixelShader  = compile ps_2_0 DepthFragment();

        CullMode = CCW;
		ZWriteEnable = TRUE;
		ZEnable = TRUE;
    }
}

technique DepthPointLight3
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthVertexPoint(3);
        pixelShader  = compile ps_2_0 DepthFragment();

        CullMode = CCW;
		ZWriteEnable = TRUE;
		ZEnable = TRUE;
    }
}

technique DepthPointLight4
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthVertexPoint(4);
        pixelShader  = compile ps_2_0 DepthFragment();

        CullMode = CCW;
		ZWriteEnable = TRUE;
		ZEnable = TRUE;
    }
}

technique DepthPointLight5
{
    pass P0
    {
        vertexShader = compile vs_2_0 DepthVertexPoint(5);
        pixelShader  = compile ps_2_0 DepthFragment();

        CullMode = CCW;
		ZWriteEnable = TRUE;
		ZEnable = TRUE;
    }
}

#endif