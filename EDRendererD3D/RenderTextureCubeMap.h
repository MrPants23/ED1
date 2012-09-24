#pragma once

#include "RenderTexture.h"

namespace EDRendererD3D
{
	class RenderTextureCubeMap : public RenderTexture
	{
	public:
		RenderTextureCubeMap(void){}

		/// Loads a texture from file.
		/// \return Returns the pointer to the RenderTextureCubeMap.
		/// \param fileName - File path to the texture file to load.
		static RenderTextureCubeMap* Load(const char* fileName);
	};
}
