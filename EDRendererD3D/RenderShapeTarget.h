#pragma once

#include "RenderShapeBase.h"
#include "RenderTarget.h"
#include "RenderContext.h"
#include "RenderMesh.h"

namespace EDRendererD3D
{
	/// RenderShapeTarget is a specialization of RenderNode used to define 
	/// a renderable using a RenderTarget as its texture.
	class RenderShapeTarget : public RenderShapeBase
	{
	private:
		/// Pointer to the RenderContext of this shape
		RenderContext *renderContextPtr;
		/// Points to the RenderMesh of this shape
		RenderMesh *renderMeshPtr;
		/// Points to the RenderTarget to be used as the texture
		RenderTarget *renderTargetPtr;

		D3DXMATRIX worldMatrix;
	public:
		RenderShapeTarget() : renderContextPtr(0), renderMeshPtr(0), renderTargetPtr(0){}

		/// Initializes the RenderShape.
		/// \param _renderMeshPtr - The address of the mesh to be used while rendering this shape.
		/// \param _renderContextPtr - The Address of the context to be used while rendering this shape.
		/// \param _renderTargetPtr - The RenderTarget whose texture should be rendered
		void Initialize(RenderMesh *_renderMeshPtr, RenderContext *_renderContextPtr, RenderTarget *_renderTargetPtr);

		/// A helper function to simplify adding shapes to their RenderSets.
		/// This method will add the shape to the RenderSet held by the shapes context.
		/// This is the way to add this shape to a render batch.
		void AddToContextSet();

		inline void SetRenderTarget(RenderTarget *_renderTargetPtr)
		{ renderTargetPtr = _renderTargetPtr; }

		inline D3DXMATRIX *GetWorldMatrixPtr() { return &worldMatrix; }
		inline RenderContext *GetRenderContext(int lod = -1) { return renderContextPtr; }
		inline D3DXMATRIX &GetWorldMatrix() { return worldMatrix; }
		inline void SetWorldMatrix(D3DXMATRIX *mat) { worldMatrix = *mat; }
		inline void SetWorldMatrix(float4x4 *mat) { SetWorldMatrix((D3DXMATRIX *)mat); }

		/// An EDRenderFunc that can render shapes whose vertices are not indexed, 
		/// no transform matrix will be set.  This is typically used for 2D rendering.
		static void PrimitiveNoTransformRenderFunc(RenderNode &node);

		/// An EDRenderFunc that can render shapes whose vertices are indexed, 
		/// no transform matrix will be set.  This is typically used for 2D rendering.
		static void IndexedPrimitiveNoTransformRenderFunc(RenderNode &node);

		/// An EDRenderFunc that can render shapes whose vertices are indexed, 
		/// a world space transform matrix will be set.  
		/// This is typically used for 2D rendering, that supports split screen.
		static void IndexedPrimitiveRenderFunc(RenderNode &node);
	};
}
