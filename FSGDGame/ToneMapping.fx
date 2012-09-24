#ifndef TONE_MAPPING_FX
#define TONE_MAPPING_FX

#include "2DPassThrough.fx"

float  MIDDLE_GREY = 0.05f;
float  LUM_WHITE = 2.0f;

float4 ToneMap(TwoDVertexOut inVert) : COLOR0
{
	//return tex2D(sourceSampler, inVert.texCoord + gHalfPixel) + 
		//tex2D(sharpSampler, inVert.texCoord + gHalfPixel);

	float4 color;
	float4 lum;

	color = tex2D(sharpSampler, inVert.texCoord + gHalfPixel);
	lum = tex2D(sourceSampler, float2(0.5, 0.5));
	//lum = 0.08f;

	color.rgb *= MIDDLE_GREY / (lum + 0.001f);
	color.rgb *= (1.0f + color/LUM_WHITE);
	color.rgb /= (1.0f + color);

	//color.rgb += 0.6f;
	color.a = 1.0f;

	return color;
}

technique PostProcess
{
    pass P0
    {
        vertexShader = compile vs_2_0 TwoDVertexWithTransform();
        pixelShader  = compile ps_2_0 ToneMap();
        ZEnable = false;
    }
}

#endif