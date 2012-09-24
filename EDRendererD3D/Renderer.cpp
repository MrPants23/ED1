#include "Renderer.h"
#include "RenderNode.h"
#include "RenderSet.h"

#include "../EDSolutions/RendererSolutions.h"

namespace EDRendererD3D
{
    IDirect3DDevice9 *Renderer::theDevicePtr = 0;
    IDirect3D9 *Renderer::theD3DPtr = 0;
    ID3DXEffectPool *Renderer::theEffectPoolPtr = 0;
	UINT Renderer::theFrameCounter = 1;

    void Renderer::Initialize(HWND hWnd)
    {
        // Implement a solution for the Renderer Lab
        return InitializeSolution(hWnd);
    }

    void Renderer::Initialize(D3DPRESENT_PARAMETERS &d3dpp)
    {
        // Implement a solution for the Renderer Lab
        return InitializeSolution(d3dpp);
    }

    void Renderer::Shutdown()
    {
        // release the d3d object and device
        if (theDevicePtr != NULL)
        {
			theDevicePtr->Release();	
            theDevicePtr = NULL;
        }
        if (theD3DPtr != NULL)
        {
            theD3DPtr->Release();	
            theD3DPtr = NULL;
        }
		if(theEffectPoolPtr)
		{
			theEffectPoolPtr->Release();
			theEffectPoolPtr = 0;
		}
    }

    void Renderer::Render(RenderSet &set)
    {
        // Implement a solution for the Renderer Lab
        return RenderSolution(set);
    }
}