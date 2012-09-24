#ifndef POST_TEST_FX
#define POST_TEST_FX

#include "2DPassThrough.fx"

float4 PostTestFS(TwoDVertexOut inVert) : COLOR0
{
	float4 color;
    float2 texCoord = inVert.texCoord + gHalfPixel;

	color = tex2D(sourceSampler, texCoord).rrba;

	return color;
}

technique PostProcess
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertex();
        pixelShader  = compile ps_2_0 PostTestFS();
        ZEnable = false;
    }
}

#endif