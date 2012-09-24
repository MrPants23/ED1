#ifndef BLOOM_H_FX
#define BLOOM_H_FX

#include "GaussianBlur.fx"

technique PostProcess
{
	pass P0
	{
		VertexShader = compile vs_3_0 BlurVS(float2(0, 1));
		PixelShader = compile ps_3_0 BlurPS(2.0f);
        ZEnable = false;
	}		
}

#endif