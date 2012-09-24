#include "Renderer.h"

#include "RenderShape.h"
using namespace EDRendererD3D;

#include "../EDSolutions/RenderShapeSolutions.h"

void RenderShape::AddToContextSet()
{
	//currentLOD = this->renderFormHandles.size()-1;
	if( renderFrameNumber < Renderer::GetFrameNumber() )
	{
		renderFrameNumber = Renderer::GetFrameNumber();
		GetRenderContext(currentLOD)->AddRenderNode(this);
	}
}

void RenderShape::Initialize(vector< ContentHandle<RenderForm> >&formHandles,
    D3DXMATRIX* localMatrixPtr)
{
	SetRenderFormHandle(formHandles);
	
	if( localMatrixPtr != 0 )
		worldMatrix = *localMatrixPtr;

	delete []renderIndexRangeFrameNumber;
	renderIndexRangeFrameNumber = 0;
	
	RenderFunc = IndexedPrimitiveRenderFunc;
}

const Sphere& RenderShape::GetSphere(void)
{
	const EDCollision::Sphere& meshSphere = GetRenderMesh()->GetSphere();
	TranslateVector( sphere.center, meshSphere.center, *((float4x4 *)&worldMatrix) );
	sphere.radius = meshSphere.radius;

	return sphere;
}

const Sphere& RenderShape::GetLocalSphere(void) const
{
	return GetRenderMesh()->GetSphere();
}

void RenderShape::PrimitiveRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return PrimitiveRenderFuncSolution(node);
}

void RenderShape::PrimitiveNoTransformRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return PrimitiveNoTransformRenderFuncSolution(node);
}

void RenderShape::IndexedPrimitiveRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return IndexedPrimitiveRenderFuncSolution(node);
}

void RenderShape::IndexedPrimitiveNoTransformRenderFunc(RenderNode &node)
{
    // Implement a solution for the Renderer Lab
    return IndexedPrimitiveNoTransformRenderFuncSolution(node);
}

void RenderShape::IndexedPrimitiveSubDivRenderFunc(RenderNode &node)
{
	return IndexedPrimitiveSubDivRenderFuncSolution(node);
}