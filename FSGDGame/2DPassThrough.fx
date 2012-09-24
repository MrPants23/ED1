#ifndef TWOD_PASS_THROUGH_FX
#define TWOD_PASS_THROUGH_FX

#include "CommonDeclarations.fx"

float4 gGUIColor = float4(1, 1, 1, 1);

/// An output vertex structure used in simple 2D shaders
struct TwoDVertexOut
{
	float4 position : POSITION0;
	float2 texCoord : TEXCOORD0;
};

/// A simple 2D passthrough vertex function.
/// \param position - The position of the vertex
/// \param texCoord - The UV of the vertex
/// \return Returns the inputed data as a TwoDVertexOut structure
TwoDVertexOut TwoDVertex(float3 position : POSITION0, float2 texCoord : TEXCOORD0)
{
	TwoDVertexOut outVert;

	outVert.position = float4(position, 1);
	outVert.texCoord = texCoord;

	return outVert;
}

TwoDVertexOut TwoDVertexWithTransform(float3 position : POSITION0, float2 texCoord : TEXCOORD0)
{
	TwoDVertexOut outVert;

	outVert.position = mul(float4(position, 1), gWorld);
	outVert.texCoord = texCoord;

	return outVert;
}

/// A simple fragment shader to do texture look ups.
/// \param inVert - A vertex containing the UV to use.
/// \return Returns the color of the texture diffuseSampler, at the specified UV.
float4 TwoDFragment(TwoDVertexOut inVert) : COLOR 
{
	return tex2D(diffuseSampler, inVert.texCoord + gHalfPixel);
}

/// A simple fragment shader to do texture look ups.
/// \param inVert - A vertex containing the UV to use.
/// \return Returns the color of the texture diffuseSampler, at the specified UV.
float4 TwoDFragmentWithColor(TwoDVertexOut inVert) : COLOR 
{
	return tex2D(diffuseSampler, inVert.texCoord + gHalfPixel) * gGUIColor;
}

technique Basic
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertex();
        pixelShader  = compile ps_2_0 TwoDFragment();

		ZWriteEnable = FALSE;
		ZEnable = FALSE;

        CullMode = CCW;
    }
}

technique GUI
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertexWithTransform();
        pixelShader  = compile ps_2_0 TwoDFragmentWithColor();

		ZWriteEnable = FALSE;
		ZEnable = FALSE;

        CullMode = CCW;

		AlphaBlendEnable = TRUE;
        DestBlend = INVSRCALPHA;
        SrcBlend = SRCALPHA;
    }
}

technique GUIAlphaTest
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertexWithTransform();
        pixelShader  = compile ps_2_0 TwoDFragmentWithColor();

		ZWriteEnable = FALSE;
		ZEnable = FALSE;

        CullMode = CCW;

		AlphaBlendEnable = TRUE;
        DestBlend = INVSRCALPHA;
        SrcBlend = SRCALPHA;

		AlphaTestEnable = TRUE;
		AlphaFunc = Greater;
		AlphaRef = 0.9;
    }
}

#endif