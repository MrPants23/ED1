#include "PointLight.h"
#include "../EDUtilities/float3.h"
#include "../EDUtilities/float4x4.h"

using EDUtilities::float3;
using EDUtilities::float4x4;
#include "RenderModel.h"
#include "ViewPortManager.h"
#include "../EDRendererD3D/DebugRenderer.h"

namespace EDRendererD3D
{
	PointLight::PointLight()
	{
		lightModelPtr = NULL;
		radius = -1.0f;
		attenuation = D3DXVECTOR3(1, 1, 1);
	}

	void PointLight::ApplyLight(ID3DXEffect *effectPtr) 
	{
		D3DXHANDLE handle;
        handle = effectPtr->GetParameterByName(name.c_str(), "Position");
        float3 pos;
		pos.x = GetWorldMatrixPtr()->_41;
		pos.y = GetWorldMatrixPtr()->_42;
		pos.z = GetWorldMatrixPtr()->_43;

        D3DXVECTOR4 position(pos.x, pos.y, pos.z, 1);
		effectPtr->SetVector(handle, &position);

        handle = effectPtr->GetParameterByName(name.c_str(), "Attenuation");
		effectPtr->SetVector(handle, &D3DXVECTOR4(attenuation, 1));

		handle = effectPtr->GetParameterByName(name.c_str(), "Range");
		effectPtr->SetFloat(handle, radius);

		// Check if the camera is inside the lit area
		float3 diff = pos - ViewPortManager::GetReference().GetActiveViewPosition();
		if(diff.dotProduct(diff) + 0.001f < (radius * radius))
			effectPtr->SetTechnique("RenderPointLightInside");
		else
			effectPtr->SetTechnique("RenderPointLightOutside");

		// Fo' dah debuggin'
		//Sphere sphere;
		//sphere.center = pos;
		//sphere.radius = radius;
		//DebugRenderer::GetInstance()->DrawSphere(sphere);

		Light::ApplyLight(effectPtr);
	}

    void PointLight::SetAttenuation(const D3DXVECTOR3 *_attenuation, float _radius) 
    { 
        attenuation = *_attenuation; 

        //radius = (-attenuation.y + sqrt( attenuation.y * attenuation.y 
        //    - 4 * attenuation.z * (attenuation.x - 255.f))) / (2 * attenuation.z);

		radius = _radius;

		D3DXMatrixScaling(lightModelPtr->GetShapes()[0]->GetWorldMatrixPtr(), 
			GetRadius(), GetRadius(), GetRadius());
    }

	void PointLight::LoadModel(const char *fileName)
    {
		Light::LoadModel(fileName);
		
		D3DXMatrixScaling(lightModelPtr->GetShapes()[0]->GetWorldMatrixPtr(), 
			GetRadius(), GetRadius(), GetRadius());
    }
}
