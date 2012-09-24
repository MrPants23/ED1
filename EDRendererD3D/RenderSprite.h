#pragma once
#include "../EDRendererD3D/RenderModel.h"

namespace EDRendererD3D
{
	class RenderSprite
	{
	public:
		RenderSprite(void);
		~RenderSprite(void);

		HRESULT LoadFromFile(const std::string &_renderModelFile, float _x = 0.0f, float _y = 0.0f, 
			float _width = 0.0f, float _height = 0.0f, float _rotation = 0.0f, float _alpha = 1.0f,
			float _red = 1.0f, float _green = 1.0f, float _blue = 1.0f);

		/// Draw call for a 2D sprite
		///\param _texture The filename of the texture to be drawn
		///\param _x The x position in screen pixel coordinates to draw the sprite
		///\param _y The y position in screen pixel coordinates to draw the sprite
		///\param _width The width at which to draw the sprite
		///\param _height The height at which to draw the sprite
		///\param _rotation Rotation in radians
		///\param _alpha, _red, _green, _blue Color modulation
		void Draw(float _x, float _y, float _width, float _height,
			float _rotation = 0.0f, float _alpha = 1.0f, float _red = 1.0f,
			float _green = 1.0f, float _blue = 1.0f);

		/// Simple draw call for 2D sprite
		void Draw();

		void SetColor(float _r, float _g, float _b, float _a);
		void SetTransform(float4x4 _transform);
		inline void SetTranslation(float3 _trans)
		{
			D3DXMATRIX *mat = renderModelPtr->GetWorldMatrixPtr();
			mat->_41 = _trans.x;
			mat->_42 = _trans.y;
			mat->_43 = _trans.z;
		}

		/// Rotate this sprite relative to its current orientation
		///\param _radians Radians to rotate this sprite
		void Rotate(float _radians);

		/// Translate this sprite relative to its current position
		///\param _trans Amount to offset this sprite
		inline void Translate(float3 _trans)
		{
			D3DXMATRIX *mat = renderModelPtr->GetWorldMatrixPtr();
			mat->_41 += _trans.x;
			mat->_42 += _trans.y;
			mat->_43 += _trans.z;
		}
	private:
		RenderModel *renderModelPtr;
		void SetTransform(float _x, float _y, float _width, float _height, float _rotation);
	};
}
