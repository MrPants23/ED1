#ifndef COLOR_EDGE_COMBINE_FX
#define COLOR_EDGE_COMBINE_FX

#include "2DPassThrough.fx"
#include "GaussianBlur.fx"

// Settings controlling the edge detection filter.
shared float EdgeWidth = 1;
shared float EdgeIntensity = 1;

// How sensitive should the edge detection be to tiny variations in the input data?
// Smaller settings will make it pick up more subtle edges, while larger values get
// rid of unwanted noise.
shared float ColorThreshold = 0.125;
shared float ColorSensitivity = 10;

float4 ColorEdgeFS(TwoDVertexOut inVert, uniform bool ColorWrite) : COLOR0
{
	float4 sceneColor, blurColor, color1, color2, color3, color4, diagonalDeltaColor;
	float2 edgeOffset, texCoord;
	float colorDelta, edgeAmount;

	texCoord = inVert.texCoord + gHalfPixel;
    edgeOffset = EdgeWidth * (gHalfPixel * 2);

	sceneColor = tex2D(sharpSampler, texCoord);
	blurColor = tex2D(sourceSampler, texCoord);
	//color1 = (abs( sceneColor - tex2D(sharpSampler, texCoord + float2(-1, -1) * edgeOffset)) - 0.5f ) * 1.2 + 0.5f;
	//color1 += (abs( sceneColor - tex2D(sharpSampler, texCoord + float2(1, 1) * edgeOffset)) - 0.5f ) * 1.2 + 0.5f;
	//color1 += (abs( sceneColor - tex2D(sharpSampler, texCoord + float2(-1, 1) * edgeOffset)) - 0.5f ) * 1.2 + 0.5f;
	//color1 += (abs( sceneColor - tex2D(sharpSampler, texCoord + float2(1, -1) * edgeOffset)) - 0.5f ) * 1.2 + 0.5f;
//
	//edgeAmount = saturate(saturate ( dot( float3(0.2125f, 0.7154f, 0.0721f), color1.rgb)) *ColorSensitivity);
	color1 = tex2D(sharpSampler, texCoord + float2(-1, -1) * edgeOffset);
	color2 = tex2D(sharpSampler, texCoord + float2( 1,  1) * edgeOffset);
	color3 = tex2D(sharpSampler, texCoord + float2(-1,  1) * edgeOffset);
	color4 = tex2D(sharpSampler, texCoord + float2( 1, -1) * edgeOffset);

	// Work out how much the color values are changing.
    diagonalDeltaColor = abs(color1 - color2) + abs(color3 - color4);

    colorDelta = dot(diagonalDeltaColor.xyz, float3(0.2125f, 0.7154f, 0.0721f));
	colorDelta = saturate((colorDelta - ColorThreshold) * ColorSensitivity);
    edgeAmount = colorDelta * EdgeIntensity * 0.5f;

	if(ColorWrite)
	{
		sceneColor *= (1 - edgeAmount);
		edgeAmount = 1-edgeAmount;
		sceneColor = float4(edgeAmount, edgeAmount, edgeAmount, 1);
	}
	else
	{
		//if(colorDelta < ColorThreshold)
			//discard;
		sceneColor.rgb = lerp(sceneColor.rgb, blurColor.rgb, edgeAmount);

	}

    return float4(sceneColor.rgb, 1);
}

technique DrawColorEdges
{
    pass P0
    {
        vertexShader = compile vs_3_0 TwoDVertex();
        pixelShader  = compile ps_3_0 ColorEdgeFS(true);
        ZEnable = false;
    }
}

technique PostProcess
{
    pass P0
    {
        vertexShader = compile vs_3_0 TwoDVertex();
        pixelShader  = compile ps_3_0 ColorEdgeFS(false);
        ZEnable = false;
    }
}

#endif