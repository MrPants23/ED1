
#ifndef COMMON_DECLARATIONS_FX
#define COMMON_DECLARATIONS_FX

/// The concatenated world view projection matrices
shared float4x4	gMVP;
/// The world matrix of the object to be rendered.
shared float4x4	gWorld;
shared float4x4	gView;
shared float4x4	gInvViewProj;
/// The position of the camera, or eye.
shared float3 gCameraPos;
/// The difference between texel and pixel
shared float2 gHalfPixel;
/// An offset to be used when comparing depths for shadow calculations
shared float gShadowDepthOffset;
/// An offset to be used to find the texels in the directional light shadow 
/// map around the one being sampled.
shared float gShadowDirOffset;
/// The distance of the near clip plane
shared float gNearDist;
/// The distance of the far clip plane
shared float gFarDist;
/// Precomputed value used to convert depth to actual distance.
/// gPlaneX = -far / (far - near)
/// distance = gPlaneY / (gPlaneX + depth)
shared float gPlaneX;
/// Precomputed value used to convert depth to actual distance.
/// gPlaneY = -far * near/ (far - near)
/// distance = gPlaneY / (gPlaneX + depth)
shared float gPlaneY;

/// Kernel used for up and down sampling x4
shared float2 TexelCoordsDownFilterX4[16];

/// An output vertex structure containing data for texturing 
/// and lighting for a spot, directional and point light.
struct BaseVertexOut
{
	float4 position		: POSITION0;
	float4 normal		: TEXCOORD0;
	float2 texCoord		: TEXCOORD1;
	/// The direction of the point light to the vertex (x,y,z) and the distance(w)
	float4 pointDirDist	: TEXCOORD2;
	/// The direction of the spot light to the vertex (x,y,z) and the distance(w)
	float4 spotDirDist	: TEXCOORD3;
	/// A vector halfway between the pointing at the camera and the point light. 
	/// Used for specular calculations.
	float3 pointHalfVec : TEXCOORD4;
	/// A vector halfway between the pointing at the camera and the spot light. 
	/// Used for specular calculations.	
	float3 spotHalfVec	: TEXCOORD5;
	/// The vertex position in spot light space.  Used for shadowmapping.
	float4 spotLightPos	: TEXCOORD6;
	/// A vector halfway between the pointing at the camera and the directional light. 
	/// Used for specular calculations.
	float3 dirHalfVec	: TEXCOORD7;
	float4 screenPos		: TEXCOORD8;
};

/// A vertex shader output structure used to plot depths.
struct DepthVertOut
{
	float4 position : POSITION;
	float2 depth	: TEXCOORD0;
};

struct GBufferFragOut
{
	float4 diffuse : COLOR0;
	float4 normal : COLOR1;
	float4 specular : COLOR2;
};

/// The collection of data needed to define a directional light
struct DirectionalLight
{
	/// If false this light will not be used
    bool		Enabled;
	/// A projection matrix defined by this lights properties, used for shadow mapping.
	float4x4	ViewProjection;
	/// The direction the light shines
    float3		Direction;
	/// The color of the diffuse component of the light.
    float4		DiffuseColor;
	/// The color of the ambient component of the light.
    float4		AmbientColor;
	/// The color of the specular component of the light.
    float4		SpecularColor;
	/// Controls how big the specular reflection is. A smaller value creates a large reflection.
	float		SpecularPower;
	/// Controls the brightness of the specualr reflection.
    float		SpecularIntensity;
};
/// The collection of data needed to define a point light
struct PointLight
{
	/// If false this light will not be used
    bool		Enabled;
	float4x4	ViewProjection[6];

	/// The position the light shines from.
    float3		Position;
	/// The color of the diffuse component of the light.
    float4		DiffuseColor;
	/// The color of the ambient component of the light.
    float4		AmbientColor;
    ///(x = constant attenuation, y = linear attenuation, z = quadratic attenuation)
	float3		Attenuation;
	/// The maximum distance this light should effect, adjusts the resulting attenuation to match
	float		Range;
	/// The color of the specular component of the light.
    float4		SpecularColor;
	/// Controls how big the specular reflection is. A smaller value creates a large reflection.
    float		SpecularPower;
	/// Controls the brightness of the specualr reflection.
    float		SpecularIntensity;
};

struct SpotLight
{
	/// If false this light will not be used
    bool		Enabled;
	/// A projection matrix defined by this lights properties, used for shadow mapping.
	float4x4	ViewProjection;
	/// The position the light shines from.
	float4		Position;
	/// The forward direction the light shines at.
	float4		Direction;
	/// The color of the diffuse component of the light.
    float4		DiffuseColor;
	/// The color of the ambient component of the light.
    float4		AmbientColor;
	/// The color of the specular component of the light.
	float4		SpecularColor;
	/// Controls how big the specular reflection is. A smaller value creates a large reflection.
	float		SpecularPower;
	/// Controls the brightness of the specualr reflection.
    float		SpecularIntensity;
    ///(x = constant attenuation, y = linear attenuation, z = quadratic attenuation)
	float3		Attenuation;
	/// The maximum distance this light should effect, adjusts the resulting attenuation to match
	float		Range;
    /// Controls how the light attenuates as ig gets further from the direction.
	/// A larger value creates a faster fall off of brightness.
	float		Exponent;
    /// Controls how wide the angle of the spot is.
	/// This is the cosine of the angle between the direction and the furthest point to be lit.
	float		Cutoff;
};

shared DirectionalLight DirLight;
shared PointLight PointLight0;
shared SpotLight SpotLight0;

/// A texture to be used to write out depths from the spot light, 
/// which are needed for shadow mapping.
shared texture directional0ShadowMap;
/// A sample used to reference the spot0ShadowMap
sampler directional0ShadowSampler = 
sampler_state
{
	Texture = <directional0ShadowMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// A texture to be used to write out depths from the spot light, 
/// which are needed for shadow mapping.
shared texture spot0ShadowMap;
/// A sample used to reference the spot0ShadowMap
sampler spot0ShadowSampler = 
sampler_state
{
	Texture = <spot0ShadowMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// A texture to be used to write out depths from the spot light, 
/// which are needed for shadow mapping.
shared texture point0ShadowMap;
/// A sample used to reference the spot0ShadowMap
sampler point0ShadowSampler = 
sampler_state
{
	Texture = <point0ShadowMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
};

/// The diffuse texture to be applied 
shared texture ambientMap;
/// The diffuse texture sampler 
sampler ambientSampler = 
sampler_state
{
	Texture = <ambientMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// The diffuse texture to be applied 
shared texture diffuseMap;
/// The diffuse texture sampler 
sampler diffuseSampler = 
sampler_state
{
	Texture = <diffuseMap>;
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

/// The diffuse texture to be applied 
shared texture normalMap;
/// The diffuse texture sampler 
sampler normalSampler = 
sampler_state
{
	Texture = <normalMap>;
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// The diffuse texture to be applied 
shared texture specularMap;
/// The diffuse texture sampler 
sampler specularSampler = 
sampler_state
{
	Texture = <specularMap>;
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// The diffuse texture to be applied
shared texture emissiveMap;
/// The diffuse texture sampler
sampler emissiveSampler = 
sampler_state
{
	Texture = <emissiveMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// The reflection texture to be applied
shared texture reflectionMap;
/// The reflection texture sampler
sampler reflectionSampler = 
sampler_state
{
	Texture = <reflectionMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
};

/// The texture diffuse result from GBuffers pass
shared texture diffuseGBuffer;
sampler diffuseGBufferSampler = 
sampler_state
{
	Texture = <diffuseGBuffer>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// The texture specular result from GBuffers pass
shared texture specularGBuffer;
sampler specularGBufferSampler = 
sampler_state
{
	Texture = <specularGBuffer>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// The texture normals result from GBuffers pass
shared texture normalGBuffer;
sampler normalGBufferSampler = 
sampler_state
{
	Texture = <normalGBuffer>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// The texture depth result from GBuffers pass
shared texture depthGBuffer;
sampler depthGBufferSampler = 
sampler_state
{
	Texture = <depthGBuffer>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// The source texture for post-process effects
texture sourceTexture;
/// The source texture sampler 
sampler sourceSampler = 
sampler_state
{
	Texture = <sourceTexture>;
	MinFilter = ANISOTROPIC;
	MagFilter = LINEAR;
	MipFilter = POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

/// A texture that should contain an in focus version of the scene
shared texture sharpTexture;
/// The diffuse texture sampler used to reference sharpTexture
sampler sharpSampler = 
sampler_state
{
	Texture = <sharpTexture>;
	MinFilter = POINT;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = CLAMP;
	AddressV = CLAMP;
};
#endif