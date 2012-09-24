#ifndef _POSTPROCESSWATER_FX_
#define _POSTPROCESSWATER_FX_

#include "2DPassThrough.fx"


Texture2D	gHeightMapTexture;
Texture2D	gNoiseTexture;
Texture2D	gCausticTexture;
Texture2D	gFoamTexture;
TextureCube	gReflectionMapTexture;

float		gElapsedTime;

float		gDefaultWaterLevel		= -10.0f;						// Default water level.
float		gMaxWaveHeight			= 0.5f;							// Max height of the wave.
int			gWaveIterations			= 8;							// Number of loops to displace waves.

float		gNoiseSpeed				= 0.5f;							// Applied against time for lookup into noise texture.
float		gHeightSpeed			= 0.05f;						// Applied against time for lookup into height map texture.

float		gFoamRate				= 5.0f;							// Rate from foam to the the depth surface.
float		gFogRate				= 0.09f;						// Determinant for how foggy underwater.
float2		gWaveScale				= {0.05f, 0.05f};				// *CONSTANT* Increase this value to have more smaller waves.

float3		gAmbientColor;											// Color of the water with no light.
float3		gSurfaceColor;											// Color of the water surface.
float3		gFogColor				= {0.75f, 0.75f, 0.75f};		// Color of the volumetric fog.
float3		gColorExtinction		= {7.0f, 30.0f, 40.0f};			// Color extinction of RGB values.

float2		gPositiveBounds			= {50.0f, 50.0f};				// The bounds of the water on the <x,z> in the positive axis.
float2		gNegativeBounds			= {-50.0f, -50.0f};				// The bounds of the water on the <x,z> in the negative axis.
float		gFoamVisibility			= 100.0f;						// The distance along the eye vector to render foam.

float		gLightDescentRate		= 50.0f;						// Light's decent rate for underwater.
float		gRefractionAmount		= 10.0f;						// How much does the refracted image "swim"?


sampler ReflectionMapSampler = sampler_state
{
	Texture = <gReflectionMapTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

sampler HeightMapSampler = sampler_state
{
	Texture = <gHeightMapTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

sampler NoiseSampler = sampler_state
{
	Texture = <gNoiseTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

sampler FoamSampler = sampler_state
{
	Texture = <gFoamTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU  = Wrap;
	AddressV  = Wrap;
};

sampler CausticSampler = sampler_state
{
	Texture = <gCausticTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	AddressU  = Wrap;
	AddressV  = Wrap;
};


// Pixel Program
float4 PostProcessWaterPS(TwoDVertexOut _input) : COLOR0
{
	// Save the color of the scene.
	float3 sceneColor = tex2D( sourceSampler, _input.texCoord ).rgb;
	float3 finalColor = sceneColor;

	// Obtain the depth of the current pixel from the depth texture.
	float pixelDepthClip = tex2D( depthGBufferSampler, _input.texCoord ).r;

	// Create the position of the pixel in homogeneous clip space.
	float4 positionClip = float4(_input.texCoord, pixelDepthClip, 1);

	// Invert the y-axis from texture coordinates to the standard cartesian coordinate system.
	positionClip.y = 1 - positionClip.y;

	// Expand the (x,y) from texture range <0,1> to homogeneous clip space range <-1,1>.
	// Note: Don't have to expand z because camera sits at 0.0f.
	positionClip.xy = 2 * positionClip.xy - 1;

	// Reconstruct back to world space.
	positionClip = mul( positionClip, gInvViewProj );
	float3 positionWorld = positionClip.xyz / positionClip.w;

	// Save out the water's current level because it will change.
	float waterLevel = gDefaultWaterLevel;
	float pixelToWaterSurfaceVectorLength = 0.0f;

	// Create a vector from the camera's position to the current pixel, save the length, and normalize the vector.
	float3 viewVectorWorld = positionWorld - gCameraPos;
	float  viewVectorWorldLength = length( viewVectorWorld );
	float3 viewVectorWorldNorm = viewVectorWorld / viewVectorWorldLength;
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// UNDER WATER
	if( gCameraPos.y < waterLevel )
	{
		// Obtain the height difference from the water level to the camera.
		float camDepthFromWaterSurface = waterLevel - gCameraPos.y;

		// Find intersection with water surface.
		float ratio = (waterLevel - gCameraPos.y) / viewVectorWorldNorm.y;
		float3 waterSurfacePointWorld = gCameraPos + viewVectorWorldNorm * ratio;

		float2 texCoord;
		float biasAmount;
		for(int loop = 0; loop < gWaveIterations; ++loop)
		{
			texCoord = (waterSurfacePointWorld.xz + viewVectorWorldNorm.xz) * gWaveScale + gElapsedTime * gNoiseSpeed;
			biasAmount = tex2D( NoiseSampler, texCoord / 128 ).r * 0.05f;

			// Create the texture coordinates for the height map.
			texCoord = (waterSurfacePointWorld.xz + viewVectorWorldNorm.xz) * gWaveScale + gElapsedTime * gHeightSpeed;
			biasAmount += tex2D( HeightMapSampler, texCoord ).r * 0.1f;

			// Update the water level and find the new intersection point with the new water surface.
			waterLevel -= biasAmount * gMaxWaveHeight;
			ratio = (waterLevel - gCameraPos.y) / viewVectorWorldNorm.y;
			waterSurfacePointWorld = gCameraPos + viewVectorWorldNorm * ratio;
		}

		// Note: May be changed to realistic caustics.
		// Create the caustic texture.
		float3 causticColor = lerp( tex2D( CausticSampler, (positionWorld.xz / 10) + gElapsedTime * 0.05f ).rgb,
									tex2D( CausticSampler, (positionWorld.xz / 10) - gElapsedTime * 0.05f ).rgb,
									0.5f);

		// Obtain the height difference from the new water level to the camera.
		camDepthFromWaterSurface = waterSurfacePointWorld.y - gCameraPos.y;
	
		// Obtain the length of the vector from the current pixel's position to the water's position.
		float pixelToWaterSurfaceVectorLength = length( positionWorld.y - waterSurfacePointWorld );
		
		// If the pixel position is less than the new water level...return with depth fog.
		if(positionWorld.y < waterSurfacePointWorld.y)
		{
			// Obtain the height difference from the water level to the pixel position.
			float pixelDepthToWaterSurface = waterSurfacePointWorld.y - positionWorld.y;
			float fogIntensity = saturate(gFogRate * viewVectorWorldLength);
			finalColor += causticColor;

			finalColor = fogIntensity * gSurfaceColor + (1 - fogIntensity) * finalColor;
			finalColor *= ( saturate(1 - pixelDepthToWaterSurface / (gLightDescentRate * 5) ) *
							saturate(1 - camDepthFromWaterSurface / gLightDescentRate) );

			// TODO: Make scene color more brown underneath.
			// Return the scene color with the depth fog.
			return float4( finalColor, 1 );
		}

		// Obtain the height difference of these two positions.
		float pixelDepthFromWaterSurface = positionWorld.y - waterSurfacePointWorld.y;
		
		// Update the direction from the camera's position to the new water point.
		viewVectorWorldNorm = normalize( waterSurfacePointWorld - gCameraPos );
		
		// Contruct the world normal from the height map.
		float height0 = tex2D( HeightMapSampler, (texCoord + float2(-1, 0) / 512) ).r;
		float height1 = tex2D( HeightMapSampler, (texCoord + float2( 1, 0) / 512) ).r;
		float height2 = tex2D( HeightMapSampler, (texCoord + float2( 0,-1) / 512) ).r;
		float height3 = tex2D( HeightMapSampler, (texCoord + float2( 0, 1) / 512) ).r;

		float3 normalWorld;
		normalWorld.x = (height0 - height1) * gMaxWaveHeight;
		normalWorld.y = 1.0f;
		normalWorld.z = (height2 - height3) * gMaxWaveHeight;
		normalWorld = normalize( normalWorld );

		// Construct the "tangent" normal using the noise texture instead of normal map.
		texCoord = waterSurfacePointWorld.xz * gWaveScale + gElapsedTime * gHeightSpeed;
		height0 = tex2D( NoiseSampler, (texCoord + float2(-1, 0) / 512) ).r;
		height1 = tex2D( NoiseSampler, (texCoord + float2( 1, 0) / 512) ).r;
		height2 = tex2D( NoiseSampler, (texCoord + float2( 0,-1) / 512) ).r;
		height3 = tex2D( NoiseSampler, (texCoord + float2( 0, 1) / 512) ).r;

		float3 normalTangent0;
		normalTangent0.x = (height0 - height1) * gMaxWaveHeight;
		normalTangent0.y = 1.0f;
		normalTangent0.z = (height2 - height3) * gMaxWaveHeight;

		// Invert the direction to give the sparkle or swimming effect water gives and sample again.
		texCoord = waterSurfacePointWorld.xz * gWaveScale - gElapsedTime * gHeightSpeed;
		height0 = tex2D( NoiseSampler, (texCoord + float2(-1, 0) / 512) ).r;
		height1 = tex2D( NoiseSampler, (texCoord + float2( 1, 0) / 512) ).r;
		height2 = tex2D( NoiseSampler, (texCoord + float2( 0,-1) / 512) ).r;
		height3 = tex2D( NoiseSampler, (texCoord + float2( 0, 1) / 512) ).r;

		float3 normalTangent1;
		normalTangent1.x = (height0 - height1) * gMaxWaveHeight;
		normalTangent1.y = 1.0f;
		normalTangent1.z = (height2 - height3) * gMaxWaveHeight;

		// Add the two and normalize to get an average.
		float3 normalTangent = normalize( normalTangent0 + normalTangent1 );
		
		// Calculate the diffuse light amount with a directional light in world space.
		float diffuseAmount = saturate( dot( -DirLight.Direction.xyz, normalWorld ) );

		// Calculate the specular light amount in world space.
		float3 lightReflectionWorld = normalize( reflect( DirLight.Direction.xyz, normalWorld ) );
		float specularWorldAmount = saturate( dot( lightReflectionWorld, -viewVectorWorldNorm ) );
		float3 specularWorldColor = DirLight.SpecularColor.rgb * pow( specularWorldAmount, DirLight.SpecularPower );

		// Calculate the specular light amount in texture space.
		float3 lightReflectionTangent = normalize( reflect( DirLight.Direction.xyz, normalTangent ) );
		float specularTangentAmount = saturate( dot( lightReflectionTangent, -viewVectorWorldNorm ) );
		float3 specularTangentColor = DirLight.SpecularColor.rgb * pow( specularTangentAmount, DirLight.SpecularPower );

		///////////////////////////////////////////////////////////////////////////////////////////
		// TODO: CHANGE.
		// Create the refraction color off of the rendered scene.
		float refractionScale = 0.005f;
		texCoord = _input.texCoord.xy;
		texCoord.x += sin( gElapsedTime + gRefractionAmount * abs( waterSurfacePointWorld.y ) ) * ( refractionScale * min( pixelDepthFromWaterSurface, 1) );
		// TODO: CHANGE.
		///////////////////////////////////////////////////////////////////////////////////////////

		// Sampler the scene color with the new texture coordinates to get a swimming effect.
		float3 refractionColor;

		// Reconstruct the new pixel into world space.
		pixelDepthClip = tex2D( depthGBufferSampler, texCoord ).r;
		positionClip = float4( texCoord, pixelDepthClip, 1 );
		positionClip.y = 1 - positionClip.y;
		positionClip.xy = 2 * positionClip.xy - 1;
		positionClip = mul( positionClip, gInvViewProj );
		positionWorld = positionClip.xyz / positionClip.w;

		// If the new position is below the water level, render the default scene.
		if(positionWorld.y < waterLevel)
			refractionColor = sceneColor;
		else
			refractionColor = tex2D( sourceSampler, texCoord ).rgb;

		// Start saving out the final color with diffuse lighting, lerp between reflection and refraction, and add specular.
		finalColor = lerp( refractionColor, gSurfaceColor, 0.5f );
		finalColor *= diffuseAmount;
		finalColor += specularWorldColor + specularTangentColor;

		// To save operations (and to keep the foam from interacting with the skybox), limit the foam from a distance.
 		if(viewVectorWorldLength < gFoamVisibility)
		{
			// Sample the foam texture depending on the height of the pixel under the water.
			texCoord = ( waterSurfacePointWorld.xz ) + gElapsedTime * 0.5f;
			finalColor = lerp( finalColor, tex2D( FoamSampler, texCoord ).rgb, saturate( 1 - pixelDepthFromWaterSurface * gFoamRate ) );
		}

		// To eliminate the immense fog at water level, cap it to a range.
		if(gCameraPos.y > ( waterLevel - abs(waterLevel - waterSurfacePointWorld.y) ) )
			return float4( finalColor, 1 );
		
		// Obtain the height difference from the water level to the pixel position.
		viewVectorWorldLength = length( waterSurfacePointWorld - gCameraPos );
		float fogIntensity = saturate(gFogRate * viewVectorWorldLength);
		float pixelDepthToWaterSurface = waterSurfacePointWorld.y - positionWorld.y;

		finalColor = fogIntensity * gSurfaceColor + (1 - fogIntensity) * finalColor;
		finalColor = finalColor * ( saturate(1 - pixelDepthToWaterSurface / (gLightDescentRate * 5) ) *
									saturate(1 - camDepthFromWaterSurface / gLightDescentRate) );

		return float4( finalColor, 1 );

	}
	// UNDER WATER
	///////////////////////////////////////////////////////////////////////////////////////////////
	// ABOVE WATER
	else if( positionWorld.y < waterLevel + gMaxWaveHeight )
	{
		// Find intersection with water surface.
		float ratio = (waterLevel - gCameraPos.y) / viewVectorWorldNorm.y;
		float3 waterSurfacePointWorld = gCameraPos + viewVectorWorldNorm * ratio;
		
		float2 texCoord;
		float biasAmount;
		for(int loop = 0; loop < gWaveIterations; ++loop)
		{
			texCoord = (waterSurfacePointWorld.xz + viewVectorWorldNorm.xz) * gWaveScale + gElapsedTime * gNoiseSpeed;
			biasAmount = tex2D( NoiseSampler, texCoord / 128 ).r * 0.05f;

			// Create the texture coordinates for the height map.
			texCoord = (waterSurfacePointWorld.xz + viewVectorWorldNorm.xz) * gWaveScale + gElapsedTime * gHeightSpeed;
			biasAmount += tex2D( HeightMapSampler, texCoord ).r * 0.1f;

			// Update the water level and find the new intersection point with the new water surface.
			waterLevel += biasAmount * gMaxWaveHeight;
			ratio = (waterLevel - gCameraPos.y) / viewVectorWorldNorm.y;
			waterSurfacePointWorld = gCameraPos + viewVectorWorldNorm * ratio;
		}

		// If the new water position is above the pixel position, then return the scene color.
		if(positionWorld.y > waterSurfacePointWorld.y)
			return float4( sceneColor, 1 );

		// Obtain the length of the vector from the current pixel's position to the water's position.
		pixelToWaterSurfaceVectorLength = length( waterSurfacePointWorld - positionWorld );

		// Obtain the height difference of these two positions.
		float pixelDepthToWaterSurface = waterSurfacePointWorld.y - positionWorld.y;

		// Update the direction from the camera's position to the new water point.
		viewVectorWorldNorm = normalize( waterSurfacePointWorld - gCameraPos );
		
		// Contruct the world normal from the height map.
		float height0 = tex2D( HeightMapSampler, (texCoord + float2(-1, 0) / 512) ).r;
		float height1 = tex2D( HeightMapSampler, (texCoord + float2( 1, 0) / 512) ).r;
		float height2 = tex2D( HeightMapSampler, (texCoord + float2( 0,-1) / 512) ).r;
		float height3 = tex2D( HeightMapSampler, (texCoord + float2( 0, 1) / 512) ).r;

		float3 normalWorld;
		normalWorld.x = (height0 - height1) * gMaxWaveHeight;
		normalWorld.y = 1.0f;
		normalWorld.z = (height2 - height3) * gMaxWaveHeight;
		normalWorld = normalize( normalWorld );

		// Construct the "tangent" normal using the noise texture instead of normal map.
		texCoord = (waterSurfacePointWorld.xz * gWaveScale + gElapsedTime * gHeightSpeed) * 0.5f;
		height0 = tex2D( NoiseSampler, (texCoord + float2(-1, 0) / 512) ).r;
		height1 = tex2D( NoiseSampler, (texCoord + float2( 1, 0) / 512) ).r;
		height2 = tex2D( NoiseSampler, (texCoord + float2( 0,-1) / 512) ).r;
		height3 = tex2D( NoiseSampler, (texCoord + float2( 0, 1) / 512) ).r;

		float3 normalTangent0;
		normalTangent0.x = (height0 - height1) * gMaxWaveHeight;
		normalTangent0.y = 1.0f;
		normalTangent0.z = (height2 - height3) * gMaxWaveHeight;

		// Invert the direction to give the sparkle or swimming effect water gives and sample again.
		texCoord = (waterSurfacePointWorld.xz * gWaveScale - gElapsedTime * gHeightSpeed) * 0.5f;
		height0 = tex2D( NoiseSampler, (texCoord + float2(-1, 0) / 512) ).r;
		height1 = tex2D( NoiseSampler, (texCoord + float2( 1, 0) / 512) ).r;
		height2 = tex2D( NoiseSampler, (texCoord + float2( 0,-1) / 512) ).r;
		height3 = tex2D( NoiseSampler, (texCoord + float2( 0, 1) / 512) ).r;

		float3 normalTangent1;
		normalTangent1.x = (height0 - height1) * gMaxWaveHeight;
		normalTangent1.y = 1.0f;
		normalTangent1.z = (height2 - height3) * gMaxWaveHeight;

		// Add the two and normalize to get an average.
		float3 normalTangent = normalize( normalTangent0 + normalTangent1 );

		// Calculate the diffuse light amount with a directional light in world space.
		float diffuseAmount = saturate( dot( -DirLight.Direction.xyz, normalWorld ) );

		// Calculate the specular light amount in world space.
		float3 lightReflectionWorld = normalize( reflect( DirLight.Direction.xyz, normalWorld ) );
		float specularWorldAmount = saturate( dot( lightReflectionWorld, -viewVectorWorldNorm ) );
		float3 specularWorldColor = DirLight.SpecularColor.rgb * pow( specularWorldAmount, DirLight.SpecularPower );

		// Calculate the specular light amount in texture space.
		float3 lightReflectionTangent = normalize( reflect( DirLight.Direction.xyz, normalTangent ) );
		float specularTangentAmount = saturate( dot( lightReflectionTangent, -viewVectorWorldNorm ) );
		float3 specularTangentColor = DirLight.SpecularColor.rgb * pow( specularTangentAmount, DirLight.SpecularPower );

		///////////////////////////////////////////////////////////////////////////////////////////
		// TODO: Create a true reflection.
		// Create the reflection color off of the reflection map.
		float3 reflectionColor = reflect( viewVectorWorld, normalWorld );
		reflectionColor = texCUBE( ReflectionMapSampler, reflectionColor ).xyz;
		// TODO: Create a true reflection.
		///////////////////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////////////////////
		// TODO: CHANGE.
		// Create the refraction color off of the rendered scene.
		float refractionScale = 0.005f;
		texCoord = _input.texCoord.xy;
		texCoord.x += sin( gElapsedTime + gRefractionAmount * abs( waterSurfacePointWorld.y ) ) * ( refractionScale * min( pixelDepthToWaterSurface, 1) );
		// TODO: CHANGE.
		///////////////////////////////////////////////////////////////////////////////////////////

		// Sampler the scene color with the new texture coordinates to get a swimming effect.
		float3 refractionColor;

		// Reconstruct the new pixel into world space.
		pixelDepthClip = tex2D( depthGBufferSampler, texCoord ).r;
		positionClip = float4( texCoord, pixelDepthClip, 1 );
		positionClip.y = 1 - positionClip.y;
		positionClip.xy = 2 * positionClip.xy - 1;
		positionClip = mul( positionClip, gInvViewProj );
		positionWorld = positionClip.xyz / positionClip.w;

		// If the new texture coordinates are above the water level, use the scene color.
		if(positionWorld.y > waterLevel)
			refractionColor = sceneColor;
		else
			refractionColor = tex2D( sourceSampler, texCoord ).rgb;

		// Sampler the caustic color.
		float3 causticColor = lerp( tex2D( CausticSampler, (positionWorld.xz / 10) + gElapsedTime * 0.05f ).rgb,
									tex2D( CausticSampler, (positionWorld.xz / 10) - gElapsedTime * 0.05f ).rgb,
									0.5f);

		// Add the caustic color to the refraction color.
		refractionColor += causticColor;

		// Ambient lighting.
		float3 ambientReflectionColor = reflectionColor * gAmbientColor;

		// Start saving out the final color, lerp between reflection and refraction.
		finalColor = lerp( refractionColor, gSurfaceColor, 0.5f );
		finalColor = lerp( reflectionColor, finalColor, 0.5f );

		// Obtain the height difference from the new water level to the camera.
		float camDepthFromWaterSurface = gCameraPos.y - positionWorld.y;
		ratio = pixelDepthToWaterSurface / camDepthFromWaterSurface;

		// Obtain the height difference from the water level to the pixel position.
		float fogIntensity = saturate(gFogRate * viewVectorWorldLength * ratio);
		finalColor = fogIntensity * gSurfaceColor + (1 - fogIntensity) * finalColor;
		
		// Take account of lighting: diffuse, ambient, and specular.
		finalColor *= diffuseAmount;
		finalColor += ambientReflectionColor + specularWorldColor + specularTangentColor;

		// To save operations (and to keep the foam from interacting with the skybox), limit the foam from a distance.
 		if(viewVectorWorldLength < gFoamVisibility)
		{
			// Sample the foam texture depending on the height of the pixel under the water.
			texCoord = ( waterSurfacePointWorld.xz ) + gElapsedTime * 0.5f;
			finalColor = lerp(finalColor, tex2D( FoamSampler, texCoord ).rgb, saturate( 1 - pixelDepthToWaterSurface * gFoamRate ) );
		}

		// TODO: Make scene color more brown underneath.
		// Return the scene color with the depth fog.
		return float4( finalColor, 1 );
	}
	// ABOVE WATER
	///////////////////////////////////////////////////////////////////////////////////////////////

	return float4( finalColor, 1 );
}


technique PostProcess
{
	pass Pass0
	{
		VertexShader = compile vs_3_0 TwoDVertex();
		PixelShader	 = compile ps_3_0 PostProcessWaterPS();
	}
}


#endif