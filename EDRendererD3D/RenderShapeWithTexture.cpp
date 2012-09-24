
#include "Renderer.h"
#include "RenderShapeWithTexture.h"
using namespace EDRendererD3D;

void RenderShapeWithTexture::Initialize(vector< ContentHandle<RenderForm> >&formHandles,
    D3DXMATRIX* localMatrixPtr)
{
	SetRenderFormHandle(formHandles);
	
	if( localMatrixPtr != 0 )
		SetWorldMatrix(*localMatrixPtr);

	delete []renderIndexRangeFrameNumber;
	renderIndexRangeFrameNumber = 0;

	RenderFunc = IndexedPrimitiveRenderFunc;
}

void RenderShapeWithTexture::SetTextures(RenderShapeWithTexture &shape)
{
    // Implement a solution for the Renderer Lab
    SetTexturesSolution(shape);
}

void RenderShapeWithTexture::PrimitiveNoTransformRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return WithTexturePrimitiveNoTransformRenderFuncSolution(node);
}

void RenderShapeWithTexture::IndexedPrimitiveRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return WithTextureIndexedPrimitiveRenderFuncSolution(node);
}

void RenderShapeWithTexture::IndexedPrimitiveSubDivRenderFunc(RenderNode &node)
{
	return WithTextureIndexedPrimitiveSubDivRenderFuncSolution(node);
}
