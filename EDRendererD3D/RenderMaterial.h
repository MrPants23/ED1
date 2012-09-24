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

#include "../EDUtilities/ContentManager.h"
#include "../EDMemoryManager/MMAllocator.h"
#include "RenderTexture.h"
#include "RenderTextureCubeMap.h"

using namespace EDUtilities;

namespace EDRendererD3D
{
	/// Representation for a material color.
	union RenderColor
	{
		struct
		{
			float Red;
			float Green;
			float Blue;
			float Alpha;
		};

		float RGBA[4];
	};
    enum RenderTextureTypes { RTT_Ambient, RTT_Diffuse, RTT_Specular, RTT_Emmisive, RTT_Normal};
	/// Implementation of a surface material, with different colors and/or texture maps for surface properties (diffuse, specular, normal, etc.)
	class RenderMaterial
	{
	private:
		std::vector<ContentHandle<RenderTexture> > renderTextureHandles;
		ContentHandle<RenderTextureCubeMap> renderTextureCubeMapHandle;
		std::vector<RenderColor> renderColors;
	public:
		RenderMaterial(void){}
		
		/// \return Returns the collection of RenderTexture handles for this material.
		std::vector<ContentHandle<RenderTexture> >& GetRTHandles(void){ return renderTextureHandles; }
		/// \return Returns the RenderTextureCubeMap handle for this material.
		ContentHandle<RenderTextureCubeMap>& GetRTCubeMapHandle(void){ return renderTextureCubeMapHandle; }
		/// \return Returns the collection of RenderColors for this material.
		std::vector<RenderColor>& GetRenderColors(void){ return renderColors; }

		bool hasTexture(void);
		/// Loads a RenderMaterial from a specified XML file.
		/// \param xmlFileNamePtr - File path to the RenderMaterial to load from XML format.
		/// \param indentifier - A unique identifier, unused in this context. Exists only to match function prototype.
		static RenderMaterial* LoadXML(const char* xmlFileNamePtr, const char* indentifier = 0);
	};
}