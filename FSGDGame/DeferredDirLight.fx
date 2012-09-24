#include "GBuffers.fx"

#ifndef DEFERRED_DIR_LIGHT_FX
#define DEFERRED_DIR_LIGHT_FX

struct DirLightVertexOut
{
	float4 position		: POSITION0;
	float2 texCoord		: TEXCOORD1;
	float4 screenPos	: TEXCOORD2;
};

DirLightVertexOut DeferredDirLightVS(float3 position : POSITION0, 
	float2 texCoord : TEXCOORD0)
{

}

float DirShadow(float2 texCoord, float depth)
{
	return 1.0f;
}

float4 DeferredDirLightFS(DirLightVertexOut inVert) : COLOR 
{

}

technique RenderDirLight
{
	pass P0
	{
		vertexShader = compile vs_3_0 DeferredDirLightVS();
        pixelShader  = compile ps_3_0 DeferredDirLightFS();
        
		CullMode = CCW;
		
		AlphaBlendEnable = TRUE;
		BlendOp = ADD;
		DestBlend = ONE;
		SrcBlend = ONE;
		ZWriteEnable = FALSE;
		ZEnable = FALSE;
    }

}

#endif