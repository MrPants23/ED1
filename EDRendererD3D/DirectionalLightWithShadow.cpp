#include "DirectionalLightWithShadow.h"
#include "../EDUtilities/float3.h"
#include "../EDUtilities/float4x4.h"
using EDUtilities::float3;
using EDUtilities::float4x4;

namespace EDRendererD3D
{
	DirectionalLightWithShadow::DirectionalLightWithShadow(void)
	{
		viewProjection = 0;
	}

    DirectionalLightWithShadow::DirectionalLightWithShadow( 
        D3DXMATRIX *_ViewProjection) : 
            viewProjection(_ViewProjection)
	{
	}

    void DirectionalLightWithShadow::ApplyLight(ID3DXEffect *effectPtr)
    {
        effectPtr->SetTexture("directional0ShadowMap", 
            renderTarget.GetTexture()->GetHandle());

        D3DXHANDLE handle;
        handle = effectPtr->GetParameterByName(name.c_str(), "ViewProjection");
        effectPtr->SetMatrix(handle, viewProjection);

        DirectionalLight::ApplyLight(effectPtr);
    }

    void DirectionalLightWithShadow::ActivateShadowTarget(ID3DXEffect *effectPtr, DWORD _targetNo)
    {
        ApplyLight(effectPtr);

        renderTarget.ActivateTarget(_targetNo);
    }

    void DirectionalLightWithShadow::RevertTarget()
    {
        renderTarget.RevertTarget();
    }

    void DirectionalLightWithShadow::CreateRenderTarget(UINT width, UINT length, 
            D3DFORMAT format)
    {
        renderTarget.Create(width, length, format);
    }
}