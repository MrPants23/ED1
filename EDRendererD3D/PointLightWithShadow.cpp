#include "PointLightWithShadow.h"
#include "../EDUtilities/float3.h"
#include "../EDUtilities/float4x4.h"
using EDUtilities::float3;
using EDUtilities::float4x4;

namespace EDRendererD3D
{
    PointLightWithShadow::PointLightWithShadow()
	{
	}

    void PointLightWithShadow::ApplyLight(ID3DXEffect *effectPtr)
    {
        effectPtr->SetTexture("point0ShadowMap", 
            renderTarget.GetTexture()->GetHandle());

        if(oldWorldMat != *GetWorldMatrixPtr())
        {
			oldWorldMat = *GetWorldMatrixPtr();
			UpdateMatrices();
		}

        D3DXHANDLE handle = effectPtr->GetParameterByName(name.c_str(), "ViewProjection");
		effectPtr->SetMatrixArray(handle, viewProjection, 6);


        PointLight::ApplyLight(effectPtr);
    }

    void PointLightWithShadow::ActivateShadowTarget(ID3DXEffect *effectPtr, DWORD _targetNo, DWORD _surfaceNo)
    {
        ApplyLight(effectPtr);

        renderTarget.ActivateTarget(_targetNo, _surfaceNo);
    }

    void PointLightWithShadow::RevertTarget()
    {
        renderTarget.RevertTarget();
    }

    void PointLightWithShadow::CreateRenderTarget(UINT size, D3DFORMAT format)
    {
        renderTarget.Create(size, format);
    }

    void PointLightWithShadow::UpdateMatrices()
	{
		D3DXMATRIX viewMat;
		D3DXVECTOR3 look, up, eye;
		eye.x = GetWorldMatrixPtr()->_41;
		eye.y = GetWorldMatrixPtr()->_42;
		eye.z = GetWorldMatrixPtr()->_43;

		D3DXMatrixPerspectiveFovLH( &projection, D3DX_PI / 2.0f, 1.0f, .1f, 100.0f);

        float4x4 worldMat;
		
		// Right
        worldMat.makeIdentity();
        worldMat.XAxis = float3(0, 0, -1);
        worldMat.YAxis = float3(0, 1, 0);
        worldMat.ZAxis = float3(1, 0, 0);
        worldMat.WAxis = *(float3 *)&eye;
        float4x4 invertMat = worldMat;
	    invertMat.transpose3x3();

	    invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
	    invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
	    invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
        viewMat = *(D3DXMATRIX *)&invertMat;

        D3DXMatrixMultiply(&viewProjection[0], &viewMat, &projection);

		// Left
        worldMat.makeIdentity();
        worldMat.XAxis = float3(0, 0, 1);
        worldMat.YAxis = float3(0, 1, 0);
        worldMat.ZAxis = float3(-1, 0, 0);
        worldMat.WAxis = *(float3 *)&eye;
        invertMat = worldMat;
	    invertMat.transpose3x3();

	    invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
	    invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
	    invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
        viewMat = *(D3DXMATRIX *)&invertMat;


        D3DXMatrixMultiply(&viewProjection[1], &viewMat, &projection);

		// Up
        worldMat.makeIdentity();
        worldMat.XAxis = float3(1, 0, 0);
        worldMat.YAxis = float3(0, 0, -1);
        worldMat.ZAxis = float3(0, 1, 0);
        worldMat.WAxis = *(float3 *)&eye;
        invertMat = worldMat;
	    invertMat.transpose3x3();

	    invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
	    invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
	    invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
        viewMat = *(D3DXMATRIX *)&invertMat;

        D3DXMatrixMultiply(&viewProjection[2], &viewMat, &projection);

		// Down
        worldMat.makeIdentity();
        worldMat.XAxis = float3(1, 0, 0);
        worldMat.YAxis = float3(0, 0, 1);
        worldMat.ZAxis = float3(0, -1, 0);
        worldMat.WAxis = *(float3 *)&eye;
        invertMat = worldMat;
	    invertMat.transpose3x3();

	    invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
	    invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
	    invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
        viewMat = *(D3DXMATRIX *)&invertMat;
        D3DXMatrixMultiply(&viewProjection[3], &viewMat, &projection);

		// Forward
        worldMat.makeIdentity();
        worldMat.WAxis = *(float3 *)&eye;
        invertMat = worldMat;
	    invertMat.transpose3x3();

	    invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
	    invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
	    invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
        viewMat = *(D3DXMATRIX *)&invertMat;

        D3DXMatrixMultiply(&viewProjection[4], &viewMat, &projection);

		// Backward
        worldMat.makeIdentity();
        worldMat.XAxis = float3(-1, 0, 0);
        worldMat.YAxis = float3(0, 1, 0);
        worldMat.ZAxis = float3(0, 0, -1);
        worldMat.WAxis = *(float3 *)&eye;
        invertMat = worldMat;
	    invertMat.transpose3x3();

	    invertMat.WAxis.x = -DotProduct(worldMat.WAxis, worldMat.XAxis);
	    invertMat.WAxis.y = -DotProduct(worldMat.WAxis, worldMat.YAxis);
	    invertMat.WAxis.z = -DotProduct(worldMat.WAxis, worldMat.ZAxis);
        viewMat = *(D3DXMATRIX *)&invertMat;

        D3DXMatrixMultiply(&viewProjection[5], &viewMat, &projection);
	}
}