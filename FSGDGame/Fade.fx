#ifndef FADE_FX
#define FADE_FX

#include "2DPassThrough.fx"

float gTheta = 0;
float gScale = 0;
float gFade = 1.0f;

/// A simple fragment shader to do texture look ups.
/// \param inVert - A vertex containing the UV to use.
/// \return Returns the color of the texture diffuseSampler, at the specified UV.
float4 SineFragment(TwoDVertexOut inVert) : COLOR 
{
	float2 texCoord = inVert.texCoord + gHalfPixel;
	texCoord.y += sin(gTheta + texCoord.x * gScale);
	return tex2D(sourceSampler, texCoord) * gFade;
}

technique PostProcess
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertex();
        pixelShader  = compile ps_2_0 SineFragment();

		ZWriteEnable = FALSE;
		ZEnable = FALSE;

        CullMode = CCW;
    }
}

#endif