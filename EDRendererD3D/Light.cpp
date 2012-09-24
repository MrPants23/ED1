#include "Light.h"
#include "RenderModel.h"
#include "RenderShapeLight.h"

namespace EDRendererD3D
{
	Light::~Light()
	{
		delete lightModelPtr;
	}

	void Light::ApplyLight(ID3DXEffect *effectPtr)
	{
		D3DXHANDLE handle;

        handle = effectPtr->GetParameterByName(name.c_str(), "DiffuseColor");
		effectPtr->SetVector(handle, &diffuseColor);

        handle = effectPtr->GetParameterByName(name.c_str(), "AmbientColor");
		effectPtr->SetVector(handle, &ambientColor);

		handle = effectPtr->GetParameterByName(name.c_str(), "SpecularColor");
		effectPtr->SetVector(handle, &specularColor);

		handle = effectPtr->GetParameterByName(name.c_str(), "SpecularPower");
        effectPtr->SetFloat(handle, specularPower);

        handle = effectPtr->GetParameterByName(name.c_str(), "SpecularIntensity");
        effectPtr->SetFloat(handle, specularIntensity);

        handle = effectPtr->GetParameterByName(name.c_str(), "Enabled");
		effectPtr->SetBool(handle, isEnabled);
	}

	void Light::AddToContextSet() 
	{ 
		lightModelPtr->GetShapes()[0]->AddToContextSet(); 
	}

	void Light::LoadModel(const char *fileName)
    {
		delete lightModelPtr;

		lightModelPtr = new (RenderModel)(fileName);

		((RenderShapeLight *)lightModelPtr->GetShapes()[0])->SetLight(this);
    }
	
	D3DXMATRIX *Light::GetWorldMatrixPtr() 
	{ 
		return lightModelPtr->GetShapes()[0]->GetWorldMatrixPtr(); 
	}
}
