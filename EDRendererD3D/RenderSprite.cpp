#include "RenderSprite.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/RenderShapeGUI.h"
#include "../FSGDGame/WorldData.h"

namespace EDRendererD3D
{
	RenderSprite::RenderSprite(void)
	{
		renderModelPtr = nullptr;
	}


	RenderSprite::~RenderSprite(void)
	{
		delete renderModelPtr;
		renderModelPtr = nullptr;
	}

	void RenderSprite::SetTransform(float _x, float _y, float _width, float _height, 
		float _rotation)
	{
		float width = _width/(float)WorldData::GetScreenWidth();
		float height = _height/(float)WorldData::GetScreenHeight();
		float x = (_x / (float)WorldData::GetScreenWidth()) * 2 - 1;
		float y = ((1 - _y / (float)WorldData::GetScreenHeight()) * 2 - 1);
		float4x4 transform( width, 0, 0, 0,
			0, height, 0, 0,
			0, 0, 0, 0, 
			x, y, 0, 1);
		transform.rotateLocalZ(_rotation);

		SetTransform(transform);
	}

	HRESULT RenderSprite::LoadFromFile(const std::string &_renderModelFile, float _x, float _y, float _width,
		float _height, float _rotation, float _alpha, float _red, float _green, float _blue)
	{
		renderModelPtr = new RenderModel(_renderModelFile.c_str());

		HRESULT hr = (renderModelPtr) ? S_OK : S_FALSE;
		if(S_OK == hr)
		{
			// Use the GUI render func
			renderModelPtr->GetShapes()[0]->RenderFunc = 
				RenderShapeGUI::IndexedGUIPrimitiveRenderFunc;

			SetTransform(_x, _y, _width, _height, _rotation);

			SetColor(_red, _green, _blue, _alpha);
		}
		return hr;
	}

	void RenderSprite::Draw(float _x, float _y, float _width, float _height, float _rotation,
		float _alpha, float _red, float _green, float _blue)
	{
		SetTransform(_x, _y, _width, _height, _rotation);

		SetColor(_red, _green, _blue, _alpha);

		Draw();
	}

	void RenderSprite::Draw()
	{
		renderModelPtr->AddToContextSets();
	}

	void RenderSprite::SetColor(float r, float g, float b, float a)
	{
		((RenderShapeGUI *)
			renderModelPtr->GetShapes()[0])->SetColor(r, g, b, a);
	}

	void RenderSprite::SetTransform(float4x4 transform)
	{
		renderModelPtr->SetWorldMatrix(transform);
	}

	void RenderSprite::Rotate(float radians)
	{
		D3DXMATRIX mat;
		// This is untested, old version was a local rotate, this might be backwards
		D3DXMatrixMultiply(renderModelPtr->GetWorldMatrixPtr(), renderModelPtr->GetWorldMatrixPtr(),
			D3DXMatrixRotationZ(&mat, radians));
	}
}
