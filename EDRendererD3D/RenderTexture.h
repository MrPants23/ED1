#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

namespace EDRendererD3D
{
	class RenderTexture
	{
	protected:
		IDirect3DBaseTexture9 *textureHandle;
	public:
		RenderTexture(void) : textureHandle(0) {}
		virtual ~RenderTexture();

		/// \return Returns the IDirect3DBaseTexture9 handle of this RenderTexture.
		IDirect3DBaseTexture9* GetHandle(void){ return textureHandle; }
        /// Sets the D3D texture handle associated with this texture object.
        void SetHandle(IDirect3DBaseTexture9 *_textureHandle){ textureHandle = _textureHandle; }
		/// Loads a texture from file.
		/// \return Returns the pointer to the RenderTexture.
		/// \param fileName - File path to the texture file to load.
		static RenderTexture* Load(const char* fileName);
	};
}