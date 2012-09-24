#ifndef GAUSSIAN_BLUR_H_FX
#define GAUSSIAN_BLUR_H_FX

#include "GaussianBlur.fx"

technique PostProcess
{
	pass P0
	{
		VertexShader = compile vs_3_0 BlurVS(float2(0, 1));
		PixelShader = compile ps_3_0 BlurPS(1);
        ZEnable = false;
	}		
}

#endif