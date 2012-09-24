#pragma once
#include "RenderNode.h"
#include "RenderForm.h"
#include "RenderContext.h"

namespace EDRendererD3D
{
	class RenderShapeBase :
		public RenderNode
	{
	public:
		RenderShapeBase(void) {}
		virtual ~RenderShapeBase(void) = 0 {}
		virtual D3DXMATRIX *GetWorldMatrixPtr() = 0;
		virtual void SetWorldMatrix(D3DXMATRIX *mat) = 0;
		virtual RenderContext *GetRenderContext(int lod = -1) = 0;
		virtual void AddToContextSet() = 0;

		virtual void Initialize(vector< ContentHandle<RenderForm> >
			&formHandles, D3DXMATRIX* localMatrixPtr = 0 ) {}

		virtual inline RenderMesh *GetRenderMesh(int lod = -1) const
		{ 
			return 0;
		}
	};
}
