#ifndef TONE_CALC_FX
#define TONE_CALC_FX

#include "2DPassThrough.fx"

//static const float4 LUM_VECTOR = float4(.299, .587, .114, 0);

float4 DownFilterX2ToneFS(TwoDVertexOut inVert) : COLOR0
{
	float4 color = 0;
	float average = 0.0f;
	float pixelSize = (gHalfPixel * 2);

	for(int x = -1; x < 1; ++x)
	{
		for(int y = -1; y < 1; ++y)
		{
			color += tex2D(sourceSampler, inVert.texCoord + float2(x, y) * pixelSize);

			//average += dot(color, LUM_VECTOR);
		}
	}
	//average /= 4;
	color/=4;
	return float4(color.rgb, 1.0f);
//	return float4(average, average, average, 1.0f);
}

float4 DownFilterX3ToneFS(TwoDVertexOut inVert) : COLOR0
{
	float4 color = 0.0f;
	float average = 0.0f;
	float pixelSize = (gHalfPixel * 2);

	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			color += tex2D(sourceSampler, inVert.texCoord + float2(x, y) * pixelSize);

			//average += color.r;
		}
	}
	//average /= 9;
	color /= 9;

	return float4(color.rgb, 1.0f);
//	return float4(average, average, average, 1.0f);
}

technique ToneX2
{
    pass P0
	<
		float fScaleX = 0.5f;
		float fScaleY = 0.5f;
	>
    {
        vertexShader = compile vs_2_0 TwoDVertexWithTransform();
        pixelShader  = compile ps_2_0 DownFilterX2ToneFS();
        ZEnable = false;
    }
}

technique ToneX3
{
    pass P0
	<
		float fScaleX = 0.3333f;
		float fScaleY = 0.3333f;
	>
    {
        vertexShader = compile vs_2_0 TwoDVertexWithTransform();
        pixelShader  = compile ps_2_0 DownFilterX3ToneFS();
        ZEnable = false;
    }
}
#endif