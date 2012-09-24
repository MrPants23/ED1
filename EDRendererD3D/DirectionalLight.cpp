#include "DirectionalLight.h"
#include "../EDUtilities/float3.h"
using EDUtilities::float3;
#include "RenderModel.h"

namespace EDRendererD3D
{
	void DirectionalLight::ApplyLight(ID3DXEffect *effectPtr) 
	{
		D3DXHANDLE handle;
        handle = effectPtr->GetParameterByName(name.c_str(), "Direction");
		D3DXVECTOR4 direction(GetWorldMatrixPtr()->_31, 
			GetWorldMatrixPtr()->_32, GetWorldMatrixPtr()->_33, 0);
		effectPtr->SetVector(handle, &direction);

        lightModelPtr->GetShapes()[0]->AddToContextSet();

		Light::ApplyLight(effectPtr);
	}
}