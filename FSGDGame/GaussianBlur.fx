#ifndef GAUSSIAN_BLUR_FX
#define GAUSSIAN_BLUR_FX

#include "CommonDeclarations.fx"

const shared float3 gBlurWeights;
const shared float gCenterBlurWeights;

struct BlurVertexOut
{
	float4 position : POSITION0;
	float2 texCoord : TEXCOORD0;
	float2 positiveOffsets[3] : TEXCOORD1;
    float2 negativeOffsets[3] : TEXCOORD4;
};

BlurVertexOut BlurVS(float3 position : POSITION0, float2 texCoord : TEXCOORD0,
	uniform float2 direction)
{
	BlurVertexOut outVert;

	// Probably going to need to add some code here...

	return outVert;
}

float4 BlurPS(BlurVertexOut inVert, uniform float ScaleUp) : COLOR0
{
	// These are to store the offset sample results
	float3x4 positiveSamples, negativeSamples;
	float4 color; // This will be the weighted sample for the center texel

	// There appears to be something missing in this region.

	return color;
}

#endif