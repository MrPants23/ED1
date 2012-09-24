#include "RenderTarget.h"
#include "Renderer.h"

#include "../EDMemoryManager/MemoryManager.h"
using namespace EDMemoryManager;

namespace EDRendererD3D
{
    RenderTarget::RenderTarget()
    {
        textureTargetPtr = 0;
        oldRenderTargetPtr = 0;
        targetNo = -1;
		surfacePtrs = 0;
    }

    RenderTarget::~RenderTarget()
    {
		if(surfacePtrs)
		{
			surfacePtrs->Release();
			surfacePtrs = 0;
		}
		delete textureTargetPtr;
    }

    void RenderTarget::Create(UINT width, UINT length, D3DFORMAT format)
    {
        // Implement a solution for the Deferred Renderer Lab
        return CreateSolution(width, length, format);
    }

    void RenderTarget::ActivateTarget(DWORD _targetNo)
    {
        // Implement a solution for the Deferred Renderer Lab
        return ActivateTargetSolution(_targetNo);
    }

    void RenderTarget::RevertTarget()
    {
        // Implement a solution for the Deferred Renderer Lab
        return RevertTargetSolution();
    }
}