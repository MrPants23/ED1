#include "SpotLight.h"
#include "../EDUtilities/float3.h"
#include "../EDUtilities/float4x4.h"

using EDUtilities::float3;
using EDUtilities::float4x4;
#include "RenderModel.h"
#include "ViewPortManager.h"
#include "../EDRendererD3D/DebugRenderer.h"

namespace EDRendererD3D
{
	void SpotLight::ApplyLight(ID3DXEffect *effectPtr) 
	{
		D3DXHANDLE handle;
   		D3DXMATRIX lightViewProj;
        D3DXMATRIX lightView;

        float4x4 invertMat;

        const float4x4 &worldMat = *(float4x4 *)GetWorldMatrixPtr();

        handle = effectPtr->GetParameterByName(name.c_str(), "Position");
        D3DXVECTOR4 positionExponent(worldMat.WAxis.x, worldMat.WAxis.y, worldMat.WAxis.z, 1);
		effectPtr->SetVector(handle, &positionExponent);

        handle = effectPtr->GetParameterByName(name.c_str(), "Direction");
		float3 dir(worldMat.ZAxis.x, worldMat.ZAxis.y, worldMat.ZAxis.z);
		dir.normalize();
        D3DXVECTOR4 directionCutOff(dir.x, dir.y, dir.z,1);
		effectPtr->SetVector(handle, &directionCutOff);

        handle = effectPtr->GetParameterByName(name.c_str(), "Attenuation");
		effectPtr->SetVector(handle, &D3DXVECTOR4(attenuation, 1));
		
		handle = effectPtr->GetParameterByName(name.c_str(), "Range");
		effectPtr->SetFloat(handle, radius);

        handle = effectPtr->GetParameterByName(name.c_str(), "Exponent");
		effectPtr->SetFloat(handle, exponent);

        handle = effectPtr->GetParameterByName(name.c_str(), "Cutoff");
		effectPtr->SetFloat(handle, cutOff);

        lightModelPtr->GetShapes()[0]->AddToContextSet();

		// Check if the camera is inside the lit area
		float3 toCamera = (ViewPortManager::GetReference().GetActiveViewPosition() 
			- *(float3 *)&positionExponent).normalize();
		if(toCamera.dotProduct(dir) > cutOff)
			effectPtr->SetTechnique("RenderSpotLightInside");
		else
			effectPtr->SetTechnique("RenderSpotLightOutside");

		// fo' dah debuggin'
		//DebugRenderer::GetInstance()->DrawCone(
		//	lightModelPtr->GetShapes()[0]->GetFramePtr()->GetWorldMatrix());

		Light::ApplyLight(effectPtr);
	}

    void SpotLight::SetCutOff(float _cutOff) 
	{ 
		cutOff = _cutOff; 
	}

	void SpotLight::SetAttenuation(const D3DXVECTOR3 *_attenuation, float _radius) 
	{ 
		attenuation = *_attenuation; 

		//float adjacent = (-attenuation.y + sqrt( attenuation.y * attenuation.y 
        //   - 4 * attenuation.z * (attenuation.x - 255.f))) / (2 * attenuation.z);
		float adjacent = radius = _radius;

		float radAngle = acos(cutOff);
		float tanr = tan(radAngle);
		float opposite = tanr * adjacent;

		D3DXMatrixScaling(lightModelPtr->GetShapes()[0]->GetWorldMatrixPtr(), opposite, opposite, adjacent);
	}
}
