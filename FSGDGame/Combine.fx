#ifndef COMBINE_FX
#define COMBINE_FX

#include "2DPassThrough.fx"

float4 Combine(TwoDVertexOut inVert) : COLOR0
{
	return tex2D(sourceSampler, inVert.texCoord + gHalfPixel) + 
		tex2D(sharpSampler, inVert.texCoord + gHalfPixel);
}

technique PostProcess
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertex();
        pixelShader  = compile ps_2_0 Combine();
        ZEnable = false;
    }
}

#endif