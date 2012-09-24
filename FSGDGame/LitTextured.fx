#include "Depth.fx"
#include "CommonVertexEffects.fx"
#include "GBuffers.fx"
#include "GBuffersBump.fx"

#ifndef LIT_TEXTURED_FX
#define LIT_TEXTURED_FX

struct ColorOut
{
	float4 diffuse : COLOR0;
	float4 normal : COLOR1;
	float4 depth : COLOR2;
};

/// A fragment shader used to color a surface based on a diffuse texture, a spot, point and directional lights
/// \param inVert - The vertex to be colored
/// \return Returns the color of the surface
ColorOut LitTexturedPS(BaseVertexOut inVert)
{
	float alpha;
	float4 normal, vTexColor;
	ColorOut outColor;

	vTexColor = tex2D(diffuseSampler, inVert.texCoord);
	alpha = vTexColor.a;
	normal = normalize(inVert.normal);
	outColor.normal = normal;

	vTexColor = ApplyDirectionalLight(DirLight, inVert.dirHalfVec, normal, vTexColor);
	vTexColor += ApplyPointLightPS(PointLight0, inVert.pointHalfVec, inVert.pointDirDist, normal, 
		vTexColor, vTexColor, vTexColor);
	vTexColor += ApplySpotLightPS(SpotLight0, inVert.spotHalfVec, inVert.spotDirDist, normal, vTexColor);
	outColor.diffuse = float4(vTexColor.xyz, alpha);

	outColor.depth = inVert.screenPos.z/inVert.screenPos.w;

	return outColor;
}

technique Basic
{
    pass P0
    {
        vertexShader = compile vs_3_0 LitTexturedVS();
        pixelShader  = compile ps_3_0 LitTexturedPS();

        CullMode = CCW;
    }
}

technique Bump
{
    pass P0
    {
        vertexShader = compile vs_3_0 LitTexturedBumpVS();
        pixelShader  = compile ps_3_0 LitTexturedPS();

        CullMode = CCW;
    }
}

technique Transparency
{
    pass P0
    {
        vertexShader = compile vs_3_0 LitTexturedVS();
        pixelShader  = compile ps_3_0 LitTexturedPS();

        CullMode = CCW;
		
		AlphaBlendEnable = TRUE;
        DestBlend = INVSRCALPHA;
        SrcBlend = SRCALPHA;
    }

}

#endif