#include "SpotLightWithShadow.h"
#include "../EDUtilities/float3.h"
#include "../EDUtilities/float4x4.h"
using EDUtilities::float3;
using EDUtilities::float4x4;

namespace EDRendererD3D
{
	SpotLightWithShadow::SpotLightWithShadow()
	{

	}

	void SpotLightWithShadow::ApplyLight(ID3DXEffect *effectPtr)
	{
        effectPtr->SetTexture("spot0ShadowMap", 
            renderTargetPtr->GetTexture()->GetHandle());

        D3DXHANDLE handle;
		D3DXMATRIX lightViewProj;

        // set the view projection for this light, used for shadows
		float4x4 invertMat;

        const float4x4 &worldMat = *(float4x4 *)GetWorldMatrixPtr();

	    invertMat = worldMat;
		
		// Remove scale before we inverse, the world matrix will still be applied, adding in scale
		float3 scale = invertMat.getScale();
		invertMat.scale(1.0f/scale.x, 1.0f/scale.y, 1.0f/scale.z);

	    invertMat.transpose3x3();
		float3 xAxis = worldMat.XAxis;
		xAxis.normalize();
		float3 yAxis = worldMat.YAxis;
		yAxis.normalize();
		float3 zAxis = worldMat.ZAxis;
		zAxis.normalize();

		invertMat.WAxis.x = -DotProduct(worldMat.WAxis, xAxis);
	    invertMat.WAxis.y = -DotProduct(worldMat.WAxis, yAxis);
	    invertMat.WAxis.z = -DotProduct(worldMat.WAxis, zAxis);

        D3DXMatrixMultiply(&lightViewProj, (D3DXMATRIX *)&invertMat, &projection);
        handle = effectPtr->GetParameterByName(name.c_str(), "ViewProjection");

        effectPtr->SetMatrix(handle, &lightViewProj);

        SpotLight::ApplyLight(effectPtr);
	}

	void SpotLightWithShadow::ActivateShadowTarget(ID3DXEffect *effectPtr, DWORD _targetNo)
	{
        ApplyLight(effectPtr);

        renderTargetPtr->ActivateTarget(_targetNo);
	}

	void SpotLightWithShadow::RevertTarget()
	{
        renderTargetPtr->RevertTarget();
	}

	void SpotLightWithShadow::CreateRenderTarget(UINT width, UINT length, 
            D3DFORMAT format)
    {
        //renderTarget.Create(width, length, format);

		// build the matrix?
		D3DXMatrixPerspectiveFovLH( &projection, cutOff * 2.0f, 1, 0.1f, 
			lightModelPtr->GetShapes()[0]->GetWorldMatrixPtr()->_33 );
    }
}