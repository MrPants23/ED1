#include "RenderShapeTarget.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	void RenderShapeTarget::Initialize(RenderMesh *_renderMeshPtr, 
        RenderContext *_renderContextPtr, RenderTarget *_renderTargetPtr)
    {
        renderMeshPtr = _renderMeshPtr;
        renderContextPtr = _renderContextPtr;
        renderTargetPtr = _renderTargetPtr;
        RenderFunc = IndexedPrimitiveNoTransformRenderFunc;

		D3DXMatrixIdentity(&worldMatrix);
    }

	void RenderShapeTarget::AddToContextSet()
    {
		renderContextPtr->AddRenderNode(this);
    }

    void RenderShapeTarget::PrimitiveNoTransformRenderFunc(RenderNode &node)
    {
        RenderShapeTarget &shape = (RenderShapeTarget &)node;
        RenderContext &context = *shape.renderContextPtr;

        context.EffectSetDiffuseMap(
            shape.renderTargetPtr->GetTexture()->GetHandle());

        for(UINT i = 0; i < shape.renderContextPtr->GetPasses(); i++)
        {
            shape.renderContextPtr->GetEffect()->BeginPass(i);

            Renderer::theDevicePtr->DrawPrimitive(shape.renderMeshPtr->GetPrimitiveType(), shape.renderMeshPtr->GetStartVertex(), 
                shape.renderMeshPtr->GetPrimitiveCount());

            shape.renderContextPtr->GetEffect()->EndPass();
        }
    }

	void RenderShapeTarget::IndexedPrimitiveNoTransformRenderFunc(RenderNode &node)
    {
        RenderShapeTarget &shape = (RenderShapeTarget &)node;
        RenderContext &context = *shape.renderContextPtr;

        context.EffectSetDiffuseMap(
            shape.renderTargetPtr->GetTexture()->GetHandle());

		float4x4 ident;
		ident.makeIdentity();
		context.GetEffect()->SetMatrix("gWorld", (D3DXMATRIX *)&ident);

        for(UINT i = 0; i < shape.renderContextPtr->GetPasses(); i++)
        {
            shape.renderContextPtr->GetEffect()->BeginPass(i);

            Renderer::theDevicePtr->DrawIndexedPrimitive(shape.renderMeshPtr->GetPrimitiveType(), shape.renderMeshPtr->GetStartVertex(), 
				0, shape.renderMeshPtr->GetVertCount(), shape.renderMeshPtr->GetStartIndex(), shape.renderMeshPtr->GetPrimitiveCount());

            shape.renderContextPtr->GetEffect()->EndPass();
        }
    }

	void RenderShapeTarget::IndexedPrimitiveRenderFunc(RenderNode &node)
    {
        RenderShapeTarget &shape = (RenderShapeTarget &)node;
        RenderContext &context = *shape.renderContextPtr;

        context.EffectSetDiffuseMap(
            shape.renderTargetPtr->GetTexture()->GetHandle());

		context.EffectSetWorld((D3DXMATRIX *)&shape.worldMatrix);
        for(UINT i = 0; i < shape.renderContextPtr->GetPasses(); i++)
        {
            shape.renderContextPtr->GetEffect()->BeginPass(i);

            Renderer::theDevicePtr->DrawIndexedPrimitive(shape.renderMeshPtr->GetPrimitiveType(), shape.renderMeshPtr->GetStartVertex(), 
				0, shape.renderMeshPtr->GetVertCount(), shape.renderMeshPtr->GetStartIndex(), shape.renderMeshPtr->GetPrimitiveCount());

            shape.renderContextPtr->GetEffect()->EndPass();
        }
    }
}