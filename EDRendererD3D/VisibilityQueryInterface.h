#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

#include "RenderTarget.h"
#include "RenderShape.h"

namespace EDRendererD3D
{
	enum VisibilityQueryResult{ VQR_Incomplete = 0, VQR_Visible, VQR_NotVisiable };
	class VisibilityQueryInterface
	{
	public:
		VisibilityQueryInterface(void);
		~VisibilityQueryInterface(void);

		void Initialize(RenderTarget *_renderTargetPtr, IDirect3DSurface9 *_depthSurfacePtr);

		void IssueQuery(D3DXMATRIX *transformMat); 

		VisibilityQueryResult GetResult(void);
		static RenderShapeBase *theShapePtr;
	private:
		IDirect3DQuery9 *queryPtr;

		IDirect3DSurface9 *depthSurfacePtr;

		RenderTarget *renderTargetPtr;

		DWORD pixelCountThreshold;
	};
}
