#include "CubeRenderTarget.h"
#include "Renderer.h"

#include "../EDMemoryManager/MemoryManager.h"
using namespace EDMemoryManager;

namespace EDRendererD3D
{
    CubeRenderTarget::CubeRenderTarget()
    {
        textureTargetPtr = 0;
        oldRenderTargetPtr = 0;
        targetNo = -1;
		for(int i = 0; i < NUM_SURFACES; ++i)
			surfacePtrs[i] = 0;
    }

    CubeRenderTarget::~CubeRenderTarget()
    {
		for(int i = 0; i < NUM_SURFACES; ++i)
		{
			if(surfacePtrs[i])
			{
				surfacePtrs[i]->Release();
				surfacePtrs[i] = 0;
			}
		}
		delete textureTargetPtr;
    }

    void CubeRenderTarget::Create(UINT size, D3DFORMAT format)
    {
    	IDirect3DCubeTexture9 *pTexture;
        Renderer::theDevicePtr->CreateCubeTexture(size, 1, 
            D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &pTexture, 0);

		textureTargetPtr = new RenderTexture;
        textureTargetPtr->SetHandle(pTexture);

		for(int i = 0; i < NUM_SURFACES; ++i)
	    ((IDirect3DCubeTexture9 *)
			textureTargetPtr->GetHandle())->GetCubeMapSurface(
            (D3DCUBEMAP_FACES)i, 0, &surfacePtrs[i] );
    }

    void CubeRenderTarget::ActivateTarget(DWORD _targetNo, DWORD _surfaceNo)
    {
        targetNo = _targetNo;

    	Renderer::theDevicePtr->GetRenderTarget(targetNo, &oldRenderTargetPtr );
        Renderer::theDevicePtr->SetRenderTarget(targetNo, 
			surfacePtrs[_surfaceNo]);
    }

    void CubeRenderTarget::RevertTarget()
    {
        Renderer::theDevicePtr->SetRenderTarget( targetNo, oldRenderTargetPtr );
        if(oldRenderTargetPtr)
	    {
            oldRenderTargetPtr->Release();
	        oldRenderTargetPtr = 0;
        }
        targetNo = -1;
    }
}