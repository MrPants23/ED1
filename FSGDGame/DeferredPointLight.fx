#include "GBuffers.fx"

#ifndef DEFERRED_POINT_LIGHT_FX
#define DEFERRED_POINT_LIGHT_FX

struct PointLightVertexOut
{
	float4 position		: POSITION0;
	float4 screenPos	: TEXCOORD0;
};

float4 StencilFS(PointLightVertexOut inVert) : COLOR0 
{
	return float4(1, 0, 0, 1);
}

PointLightVertexOut DeferredPointLightVS(float3 position : POSITION0)
{

}

float4 DeferredPointLightFS(PointLightVertexOut inVert) : COLOR 
{

}

// The way to render a point light that the camera is outside of
technique RenderPointLightOutside
{
	pass P0
	{
		vertexShader = compile vs_3_0 DeferredPointLightVS();
        pixelShader  = compile ps_3_0 StencilFS();

		ZWriteEnable = FALSE;
		ZEnable = TRUE;
		ZFunc = GREATER;

		COLORWRITEENABLE = 0;
		AlphaBlendEnable = FALSE;

		CullMode = CW;
		StencilEnable = TRUE;
		StencilPass = REPLACE;
		StencilRef = 0x1;
	}
	pass P1
	{
		vertexShader = compile vs_3_0 DeferredPointLightVS();
        pixelShader  = compile ps_3_0 StencilFS();

		ZWriteEnable = FALSE;
		ZEnable = TRUE;
		ZFunc = LESSEQUAL;
		COLORWRITEENABLE = 0;
		AlphaBlendEnable = FALSE;

		CullMode = CCW;
		StencilEnable = TRUE;
		StencilPass = INCR;
		StencilFunc = EQUAL;
		StencilRef = 0x1;
	}
	pass P2
	{
		vertexShader = compile vs_3_0 DeferredPointLightVS();
        pixelShader  = compile ps_3_0 DeferredPointLightFS();
        
		CullMode = CCW;
		
		AlphaBlendEnable = TRUE;
		BlendOp = ADD;
		DestBlend = ONE;
		SrcBlend = ONE;

		ZWriteEnable = FALSE;
		ZEnable = FALSE;

		COLORWRITEENABLE = 0x0000000F;

		StencilEnable = TRUE;
		StencilFunc = EQUAL;
		StencilPass = KEEP;
		StencilRef = 0x2;
    }
}

// The way to render a point light that the camera is within
technique RenderPointLightInside
{
	pass P0
	{
		vertexShader = compile vs_3_0 DeferredPointLightVS();
        pixelShader  = compile ps_3_0 StencilFS();

		ZWriteEnable = FALSE;
		ZEnable = TRUE;
		ZFunc = GREATER;

		COLORWRITEENABLE = 0;
		AlphaBlendEnable = FALSE;

		CullMode = CW;
		StencilEnable = TRUE;
		StencilPass = REPLACE;
		StencilRef = 0x1;
	}
	pass P1
	{
		vertexShader = compile vs_3_0 DeferredPointLightVS();
        pixelShader  = compile ps_3_0 DeferredPointLightFS();
        
		CullMode = CW;
		
		AlphaBlendEnable = TRUE;
		BlendOp = ADD;
		DestBlend = ONE;
		SrcBlend = ONE;

		ZWriteEnable = FALSE;
		ZEnable = FALSE;

		COLORWRITEENABLE = 0x0000000F;

		StencilEnable = TRUE;
		StencilFunc = EQUAL;
		StencilPass = KEEP;
		StencilRef = 0x1;
    }
}

#endif