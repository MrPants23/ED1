#pragma once

#include "RenderShapeBase.h"
#include "../EDMemoryManager/MemoryManager.h"
#include <vector>
using std::vector;

namespace EDRendererD3D
{
	/// RenderShape is a specialization of RenderNode used to define a renderable object.
	class RenderShape : public RenderShapeBase
	{
	protected:

		/// Handle to this shape's RenderForms, if loaded from file.
		/// Each handle represents a different LOD with the highest resolution 
		/// being first.
		vector< ContentHandle<RenderForm> > renderFormHandles;

		D3DXMATRIX worldMatrix;

		float zDepth;

		Sphere sphere;

		unsigned int* renderIndexRangeFrameNumber;
		unsigned int renderFrameNumber;

		unsigned int currentLOD;

	public:
		RenderShape() : /*m_pContext(0), m_pMesh(0), */
		  renderIndexRangeFrameNumber(0), renderFrameNumber(0), currentLOD(0) {}

		  virtual ~RenderShape()
		  {
			  if(renderIndexRangeFrameNumber)
				delete[] renderIndexRangeFrameNumber;
		  }

		  /// Initializes the RenderShape with LODs.
		  /// \param formHandles - The list of handles to the RenderForm that 
		  /// contains the mesh, material, and context used to render this shape 
		  /// for each LOD.
		  /// \param localMatrixPtr - The optional local transform used to render 
		  /// this shape.
		  virtual void Initialize(vector< ContentHandle<RenderForm> >
			  &formHandles, D3DXMATRIX* localMatrixPtr = 0 );

		  /// A helper function to simplify adding shapes to their RenderSets.
		  /// This method will add the shape to the RenderSet held by the shapes context.
		  /// This is the way to add this shape to a render batch.
		  void AddToContextSet();

		  /// \return Returns a bounding sphere that encloses this shape.
		  const Sphere& GetSphere(void);
		  const Sphere& GetLocalSphere(void) const;

		  inline void SetWorldMatrix(D3DXMATRIX &mat) { worldMatrix = mat; }
		  inline void SetWorldMatrix(D3DXMATRIX *mat) { worldMatrix = *mat; }
		  inline D3DXMATRIX &GetWorldMatrix(void) { return worldMatrix; }
		  inline D3DXMATRIX *GetWorldMatrixPtr(void) { return &worldMatrix; }

		  inline void SetZDepth(float _zDepth) { zDepth = _zDepth; }
		  inline float GetZDepth(void) const { return zDepth; }

		  //void SetRenderFormHandle(ContentHandle<RenderForm> &handle);
		  inline void SetRenderFormHandle(vector< ContentHandle<RenderForm> > &handles)
		  {
			  renderFormHandles = handles;
		  }
		  inline const RenderContext *GetRenderContext(int lod = -1) const 
		  {
			  if(lod == -1)
				  lod = currentLOD;
			  return renderFormHandles[lod].GetContent()->GetRenderContextHandle().GetContent();
		  }
		  inline RenderContext *GetRenderContext(int lod = -1) 
		  { 
			  if(lod == -1)
				  lod = currentLOD;
			  return renderFormHandles[lod].GetContent()->GetRenderContextHandle().GetContent();
		  }

		  unsigned int* GetRenderIndexRangeFrameNumbers(void){ return renderIndexRangeFrameNumber; }

		  inline RenderMesh *GetRenderMesh(int lod = -1) const
		  { 
			  if(lod == -1)
				  lod = currentLOD;
			  return renderFormHandles[lod].GetContent()->GetMeshHandle().GetContent();
		  }

		  inline size_t GetNumLODs(void) { return renderFormHandles.size(); }
		  inline void SetCurrentLOD(unsigned int lod) { currentLOD = lod; }
		  inline size_t GetCurrentLOD(void) { return currentLOD; }

		  /// An EDRenderFunc that can render shapes whose vertices are not indexed.
		  static void PrimitiveRenderFunc(RenderNode &node);
		  /// An EDRenderFunc that can render shapes whose vertices are not indexed, 
		  /// no transform matrix will be set.  This is typically used for 2D rendering.
		  static void PrimitiveNoTransformRenderFunc(RenderNode &node);
		  /// An EDRenderFunc that does nothing.  
		  /// This can be used as a placeholder or in cases when we do not want shapes in view to render.
		  static void DoNotRender(RenderNode &node) {}
		  /// An EDRenderFunc that can render shapes whose vertices are indexed.
		  static void IndexedPrimitiveRenderFunc(RenderNode &node);
		  /// An EDRenderFunc that can render shapes whose vertices are not indexed, 
		  /// no transform matrix will be set.  This is typically used for 2D rendering.
		  static void IndexedPrimitiveNoTransformRenderFunc(RenderNode &node);
		  static void IndexedPrimitiveSubDivRenderFunc(RenderNode &node);
	};
}