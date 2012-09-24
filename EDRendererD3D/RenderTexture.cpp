#include "RenderTexture.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;
#include "../EDMemoryManager/MMAllocator.h"
#include "../EDRendererD3D/Renderer.h"

namespace EDRendererD3D
{
	RenderTexture::~RenderTexture()
	{
		if(textureHandle != 0)
		{
			textureHandle->Release();
			textureHandle = 0;
		}
	}

	RenderTexture* RenderTexture::Load(const char* pInFileName)
	{
		string temp = ContentManager::theContentPath;
		temp += pInFileName;
		const char* fileName = temp.c_str();

		InternalOutput::GetReference() << "Loading " << fileName << "...\n";

		WCHAR *buffer = new WCHAR[strlen(fileName)+1];

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fileName, 
			static_cast<int>(strlen(fileName)), buffer, static_cast<unsigned int>(strlen(fileName)));

		buffer[strlen(fileName)] = 0;

		RenderTexture* pRenderTexture = new RenderTexture;

		//D3DXCreateTextureFromFile(Renderer::theDevicePtr, buffer, (IDirect3DTexture9 **)&pRenderTexture->textureHandle);
		if(S_OK != D3DXCreateTextureFromFileEx(Renderer::theDevicePtr, buffer, 
			D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, 
			D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 
			0, NULL, NULL, (IDirect3DTexture9 **)&pRenderTexture->textureHandle))
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file exist?\n", 
				fileName);
			return 0;
		}
    
		delete[] buffer;

		return pRenderTexture;
	}
}