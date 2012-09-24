#pragma once

#include "RenderShape.h"
#include "RenderMaterial.h"
#include "RenderForm.h"
#include "../EDUtilities/ContentManager.h"

namespace EDRendererD3D
{
    /// A specialization of RenderShape that allows for a texture per shape.
    /// Setting the texture per shape is slower than doing it per context but can be 
    /// required while rendering certain shapes.  One example would be a batch of transparent
    /// objects that are depth sorted.
	class RenderShapeWithTexture : public RenderShape
	{
	private:
		
        static void SetTextures(RenderShapeWithTexture &shape);
        static void SetTexturesSolution(RenderShapeWithTexture &shape);
	public:
		inline const RenderMaterial *GetRenderMaterial(unsigned int lod = -1) const 
        {
			if(lod == -1)
				lod = currentLOD;
			return renderFormHandles[lod].GetContent()->GetMaterialHandle().GetContent();
        }

		inline RenderMaterial *GetRenderMaterial(unsigned int lod = -1)  
        {
			if(lod == -1)
				lod = currentLOD;
			return renderFormHandles[lod].GetContent()->GetMaterialHandle().GetContent();
        }

		/// Initializes the RenderShape with LODs.
		/// \param formHandles - The list of handles to the RenderForm that 
        /// contains the mesh, material, and context used to render this shape 
        /// for each LOD.
		/// \param localMatrixPtr - The optional local transform used to render 
        /// this shape.
        void Initialize(vector< ContentHandle<RenderForm> >
			&formHandles, D3DXMATRIX* localMatrixPtr = 0 );

        /// An EDRenderFunc that can render shapes whose vertices are not indexed.
        /// The texture handles will be set in this method, 
        /// which is not as fast as setting them in the context.
        static void PrimitiveNoTransformRenderFunc(RenderNode &node);
        /// An EDRenderFunc that can render shapes whose vertices are indexed.
        /// The texture handles will be set in this method, 
        /// which is not as fast as setting them in the context.
        static void IndexedPrimitiveRenderFunc(RenderNode &node);

		static void IndexedPrimitiveSubDivRenderFunc(RenderNode &node);

        static void WithTexturePrimitiveNoTransformRenderFuncSolution(RenderNode &node);
        static void WithTextureIndexedPrimitiveRenderFuncSolution(RenderNode &node);
        static void WithTextureIndexedPrimitiveSubDivRenderFuncSolution(RenderNode &node);

	};
}