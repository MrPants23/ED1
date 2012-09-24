#ifndef DEBUG_LINE_FX
#define DEBUG_LINE_FX

float4x4	gMVP;
float4 gColor;

struct OutputVS
{
    float4 posW:POSITION0;
    float4 color:COLOR0;
};

OutputVS TransformWColorVS(float3 position:POSITION0, float3 col:COLOR0)
{
	// output structure
	OutputVS outVS;

	float4 temp = float4(position, 1.0f);
	
	outVS.posW = mul(temp, gMVP);
	outVS.color = float4(col, 1);
	
    return outVS;
}

float4 TransformWColorPS(float4 color:COLOR0):COLOR0
{
	return color;
}

float4 TransformVS(float3 position:POSITION0):POSITION0
{
	return mul(float4(position, 1.0f), gMVP);
}

float4 TransformPS():COLOR0
{
	return gColor;
}

technique VertexColor
{
    pass P0
    {
        vertexShader = compile vs_2_0 TransformWColorVS();
        pixelShader  = compile ps_2_0 TransformWColorPS();
    }
}

technique GlobalColor
{
    pass P0
    {
        vertexShader = compile vs_2_0 TransformVS();
        pixelShader  = compile ps_2_0 TransformPS();
    }
}

#endif