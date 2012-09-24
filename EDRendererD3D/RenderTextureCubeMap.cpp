#include "RenderTextureCubeMap.h"

#include "../EDMemoryManager/MMAllocator.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;

namespace EDRendererD3D
{
	RenderTextureCubeMap* RenderTextureCubeMap::Load(const char* pInFileName)
	{
		string temp = ContentManager::theContentPath;
		temp += pInFileName;
		const char* fileName = temp.c_str();

		InternalOutput::GetReference() << "Loading " << fileName << "...\n";

		WCHAR *buffer = new WCHAR[strlen(fileName)+1];

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fileName, 
			static_cast<int>(strlen(fileName)), buffer, static_cast<int>(strlen(fileName)));

		buffer[strlen(fileName)] = 0;

		RenderTextureCubeMap* pRenderTextureCubeMap = new RenderTextureCubeMap;

		if(S_OK != D3DXCreateCubeTextureFromFile(Renderer::theDevicePtr, buffer, 
			(IDirect3DCubeTexture9 **)&pRenderTextureCubeMap->textureHandle))
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file exist?\n", 
				fileName);
			return 0;
		}
    
		delete[] buffer;

		return pRenderTextureCubeMap;	
	}
}