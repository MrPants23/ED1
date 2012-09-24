//per Vertex and per Pixel algorythms
//written by Tim Turcich
//Saturday Feb 2010

//transform specifics
uniform float4x4 WorldMat;
uniform float4x4 ViewMat;
uniform float4x4 ProjMat;
uniform float4x4 WorldViewMat;
uniform float4x4 WorldViewProjMat;

//camera specifics
uniform float3 	 CameraPosVec;

//Single PointLight specifics
uniform float3   PointLightPosVec;
uniform float	 PointLightRange;
uniform float4	 PointLightColor;
uniform float4	 PointLightSpecColor;
uniform float    PointLightSpecPower;

//Color specifics
uniform float4	 Colour;

//Texture specifics
uniform texture  MainTexture;

sampler MainTextureSample = sampler_state
{
	Texture = MainTexture;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = BORDER;
	AddressV = BORDER;
};

struct VS_INPUT
{
	vector position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	/*Never use NORMAL in here*/
	vector position : POSITION;
	float3 pos3D : TEXCOORD0;
	float3 normal3D : TEXCOORD1;
	float2 tex : TEXCOORD2;
	float4 color : COLOR;
};

VS_OUTPUT PolyColorVertex(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, WorldViewProjMat);
	output.pos3D = mul(input.position, WorldMat);
	output.normal3D = normalize(mul(input.normal, WorldMat));
	output.tex = input.tex;
	output.color = input.color;
	return output;
}

VS_OUTPUT LineVertex(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.position = mul(input.position, WorldViewProjMat);
	//output.pos3D = mul(input.position, WorldMat);
	//output.normal3D = normalize(mul(input.normal, WorldMat));
	//output.tex = input.tex;
	//output.color = input.color;
	return output;
}

float4 PolyColorPixelCW(VS_OUTPUT input) : COLOR
{
	//float NdotL;
	//float NdotHV;
	//float4 finalPixelcolor = float4(0.0f,0.0f,0.0f,0.0f);
	//float3 toCamera = normalize(CameraPosVec - input.pos3D);/*direction to the camera*/
	//float3 toLight = PointLightPosVec - input.pos3D;
	//float lightDist = length(toLight);
	//float atten = max((PointLightRange - lightDist)/PointLightRange,0.0f);/*linear attenuation*/
	
	//toLight = normalize(toLight);
	//input.normal3D = normalize(input.normal3D);/*renormalize*/
	//NdotL = max(dot(input.normal3D,toLight),0.0f);
	//NdotHV = max(dot(input.normal3D, normalize(toLight + toCamera)),0.0001f);/*clamp to 0.0f if negative*/
	//finalPixelcolor += atten * ( ((PointLightColor*input.color) + (PointLightSpecColor * pow(NdotHV,PointLightSpecPower))) * NdotL );
	return Colour;
}

float4 PolyColorPixelCCW(VS_OUTPUT input) : COLOR
{
	//float NdotL;
	//float NdotHV;
	//float4 finalPixelcolor = float4(0.0f,0.0f,0.0f,0.0f);
	//float3 toCamera = normalize(CameraPosVec - input.pos3D);/*direction to the camera*/
	//float3 toLight = PointLightPosVec - input.pos3D;
	//float lightDist = length(toLight);
	//float atten = max((PointLightRange - lightDist)/PointLightRange,0.0f);/*linear attenuation*/
	
	//toLight = normalize(toLight);
	//input.normal3D = -normalize(input.normal3D);/*renormalize*/
	//NdotL = max(dot(input.normal3D,toLight),0.0f);
	//NdotHV = max(dot(input.normal3D, normalize(toLight + toCamera)),0.0001f);/*clamp to 0.0f if negative*/
	//finalPixelcolor += atten * ( ((PointLightColor*input.color) + (PointLightSpecColor * pow(NdotHV,PointLightSpecPower))) * NdotL );
	return Colour;
}

float4 LinePixel(VS_OUTPUT input) : COLOR
{
	return Colour;
}

technique SinglePolygon
{
	pass p0
	{
		ZEnable = false;
		CullMode	= CW;
		AlphaBlendEnable	= true;
		SrcBlend			= SrcAlpha;
		DestBlend			= InvSrcAlpha;
		
		vertexShader = compile vs_2_0 PolyColorVertex();
		pixelShader  = compile ps_2_0 PolyColorPixelCW();
	}
	pass p1
	{
		ZEnable = false;
		CullMode	= CCW;
		AlphaBlendEnable	= true;
		SrcBlend			= SrcAlpha;
		DestBlend			= InvSrcAlpha;
		
		vertexShader = compile vs_2_0 PolyColorVertex();
		pixelShader  = compile ps_2_0 PolyColorPixelCCW();
	}
}

technique Lines
{
	pass p0
	{
		ZEnable = false;
		CullMode	= NONE;
		vertexShader = compile vs_2_0 LineVertex();
		pixelShader  = compile ps_2_0 LinePixel();
	}
}