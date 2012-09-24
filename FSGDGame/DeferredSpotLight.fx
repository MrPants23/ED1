#include "CommonDeclarations.fx"

#ifndef DEFERRED_SPOT_LIGHT_FX
#define DEFERRED_SPOT_LIGHT_FX

struct SpotLightVertexOut
{
	float4 position		: POSITION0;
	float4 screenPos	: TEXCOORD0;
};

SpotLightVertexOut DeferredSpotLightVS(float3 position : POSITION0)
{
	SpotLightVertexOut outVert;
	
	outVert.position = mul(float4(position, 1.0), gMVP);
	outVert.position.z = min(outVert.position.w, outVert.position.z);
	outVert.screenPos = outVert.position;

	return outVert;
}

float4 StencilFS(SpotLightVertexOut inVert) : COLOR0 
{
	return float4(1, 0, 0, 1);
}

float4 DeferredSpotLightFS(SpotLightVertexOut inVert) : COLOR0 
{
	float4 diffuse, specular, normal, depth, posScreen, posWorld, 
		lightSpacePos, finalAmbient, finalDiffuse, finalSpecular;
	float3 toLight, texCubeCoords, reflectionVector, directionToCamera;
	float2 texCoord;
	float toLightLength, attenuation, nDotL, lightMapDepth, specMod,
		spotEffect, shadow;
	int lightViewProjIndex;

	// Insert your solution here.  Feel free to replace my variables.

	return attenuation * (finalAmbient + shadow * (finalDiffuse + finalSpecular));
}

technique RenderSpotLightOutside
{
	pass P0
	{
		vertexShader = compile vs_3_0 DeferredSpotLightVS();
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
		vertexShader = compile vs_3_0 DeferredSpotLightVS();
        pixelShader  = compile ps_3_0 StencilFS();

		ZWriteEnable = FALSE;
		ZEnable = TRUE;
		ZFunc = LESSEQUAL;

		AlphaBlendEnable = FALSE;

		CullMode = CCW;
		StencilEnable = TRUE;
		StencilPass = INCR;
		StencilFunc = EQUAL;
		StencilRef = 0x1;
	}
	pass P2
	{
		vertexShader = compile vs_3_0 DeferredSpotLightVS();
        pixelShader  = compile ps_3_0 DeferredSpotLightFS();
        
		CullMode = CCW;
		
		COLORWRITEENABLE = 0x0000000F;

		AlphaBlendEnable = TRUE;
		BlendOp = ADD;
		DestBlend = ONE;
		SrcBlend = ONE;

		ZWriteEnable = FALSE;
		ZEnable = FALSE;

		StencilEnable = TRUE;
		StencilFunc = EQUAL;
		StencilPass = KEEP;
		StencilRef = 0x2;
    }

}

technique RenderSpotLightInside
{
	pass P0
	{
		vertexShader = compile vs_3_0 DeferredSpotLightVS();
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
		vertexShader = compile vs_3_0 DeferredSpotLightVS();
        pixelShader  = compile ps_3_0 DeferredSpotLightFS();
        
		CullMode = CW;
		
		COLORWRITEENABLE = 0x0000000F;

		AlphaBlendEnable = TRUE;
		BlendOp = ADD;
		DestBlend = ONE;
		SrcBlend = ONE;

		ZWriteEnable = FALSE;
		ZEnable = FALSE;

		StencilEnable = TRUE;
		StencilFunc = EQUAL;
		StencilPass = KEEP;
		StencilRef = 0x1;
    }

}

#endif