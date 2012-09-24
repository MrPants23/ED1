#include "RenderShapeLight.h"
#include "ViewPortManager.h"
#include "Renderer.h"

#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/IndexBuffer.h"

namespace EDRendererD3D
{
	RenderShapeLight::RenderShapeLight(void)
	{
	}


	RenderShapeLight::~RenderShapeLight(void)
	{
	}

	void RenderShapeLight::IndexedPrimitiveRenderFunc(RenderNode &node)
	{
		RenderShapeLight &shape = (RenderShapeLight &)node;
		// Apply the light
		shape.lightPtr->ApplyLight(shape.GetRenderContext()->GetEffect());
		//shape.GetRenderContext()->GetEffect()->CommitChanges();

		// Stupid hack attempt to do all context stuff again
		// I should not have to begin and end the effect here, this code is happening inbetween a begin and end already...
		RenderContext &context = *shape.GetRenderContext();

		//context.GetEffect()->Begin(&context.GetPasses(), 0);
		RenderShape::IndexedPrimitiveRenderFunc(node);
		//context.GetEffect()->End();
	}

	void RenderShapeLight::Initialize(vector< ContentHandle<RenderForm> >&formHandles,
		D3DXMATRIX* localMatrixPtr)
	{
		RenderShape::Initialize(formHandles, localMatrixPtr);

		RenderFunc = RenderShapeLight::IndexedPrimitiveRenderFunc;
	}
}
