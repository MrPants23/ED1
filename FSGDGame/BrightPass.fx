#ifndef BRIGHT_PASS
#define BRIGHT_PASS

#include "2DPassThrough.fx"


float gLuminance = 0.08f;
static const float sMiddleGray = 0.18f;
static const float sWhiteCutoff = 0.8f;

float4 BrightPassFS(TwoDVertexOut inVert) : COLOR0
{
	float4 color = tex2D(sourceSampler, inVert.texCoord + gHalfPixel);

    color *= sMiddleGray / ( gLuminance + 0.001f );
    color *= ( 1.0f + ( color / ( sWhiteCutoff * sWhiteCutoff ) ) );
    color -= 5.0f;

    color = max( color, 0.0f );

    color /= ( 10.0f + color );

	return color;
}

technique PostProcess
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertex();
        pixelShader  = compile ps_2_0 BrightPassFS();
        ZEnable = false;
    }
}

#endif