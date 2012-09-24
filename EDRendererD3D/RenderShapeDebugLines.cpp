#include "RenderShapeDebugLines.h"
#include "ViewPortManager.h"
#include "Renderer.h"
#include "VertexBufferManager.h"

namespace EDRendererD3D
{
	RenderShapeDebugLines::RenderShapeDebugLines(void)
	{
	}


	RenderShapeDebugLines::~RenderShapeDebugLines(void)
	{
	}

	void RenderShapeDebugLines::AddLines(const VERTEX_POSCOLOR *_points, UINT primCount)
	{
		for(UINT i = 0; i < primCount*2; ++i)
		{
			points.push_back(_points[i]);
		}
	}

	void RenderShapeDebugLines::ClearLines(void)
	{
		points.clear();
	}

	void RenderShapeDebugLines::RenderDebugLines(RenderNode &node)
	{
		RenderShapeDebugLines &shape = (RenderShapeDebugLines &)node;
		if(0 == shape.points.size())
			return;

		ID3DXEffect *effect = shape.GetRenderContext()->GetEffect();
		// Use the vertex color technique
		//effect->SetTechnique("VertexColor");

		effect->SetMatrix("gMVP", 
			(D3DXMATRIX *)&ViewPortManager::GetReference().GetActiveViewProjection());

		// basic vertex declaration
		Renderer::theDevicePtr->SetVertexDeclaration(
			VertexBufferManager::GetReference().GetPositionColorBuffer().GetVertexDeclaration());

		//UINT passes;
		//effect->Begin(&passes, 0);
		for(UINT i = 0; i < shape.GetRenderContext()->GetPasses(); ++i)
		{
			effect->BeginPass(i);
			Renderer::theDevicePtr->DrawPrimitiveUP(D3DPT_LINELIST, static_cast<unsigned int>(shape.points.size()/2), &shape.points[0], 
				sizeof(VERTEX_POSCOLOR));
			effect->EndPass();
		}
		//effect->End();

		shape.ClearLines();
	}
}