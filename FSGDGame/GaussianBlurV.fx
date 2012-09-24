#ifndef GAUSSIAN_BLUR_V_FX
#define GAUSSIAN_BLUR_V_FX

#include "GaussianBlur.fx"

technique PostProcess
{
	pass P0
	{
		VertexShader = compile vs_3_0 BlurVS(float2(1, 0));
		PixelShader = compile ps_3_0 BlurPS(1);
        ZEnable = false;
	}		
}

#endif