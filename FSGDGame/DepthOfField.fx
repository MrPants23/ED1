#ifndef DEPTH_OF_FIELD_FX
#define DEPTH_OF_FIELD_FX

#include "2DPassThrough.fx"
#include "GBuffers.fx"

float gFocusDistance = 5.0f;
float gFocusRange = 3.0f;

float4 DepthOfFieldFS(TwoDVertexOut inVert) : COLOR0
{
	float4 sharpColor, blurColor, finalColor;
	float depth, camDepth, blur;
	float2 texCoord = inVert.texCoord + gHalfPixel;


	return finalColor;
}

technique PostProcess
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertex();
        pixelShader  = compile ps_2_0 DepthOfFieldFS();
    }
}

#endif