#ifndef BLOOM_V_FX
#define BLOOM_V_FX

#include "GaussianBlur.fx"

technique PostProcess
{
	pass P0
	{
		VertexShader = compile vs_3_0 BlurVS(float2(1, 0));
		PixelShader = compile ps_3_0 BlurPS(2.0f);
        ZEnable = false;
	}		
}

#endif