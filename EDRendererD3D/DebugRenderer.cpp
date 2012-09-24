#include "DebugRenderer.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDRendererD3D/RenderModel.h"
#include "../EDCollision/CollisionLibrary.h"
#include "../FSGDGame/RenderController.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/RenderShapeDebugLines.h"
using namespace EDRendererD3D;

DebugRenderer* DebugRenderer::instancePtr = 0;

DebugRenderer::DebugRenderer(void)
{
	sphereModelPtr = new (EDRendererD3D::RenderModel)("GDModel/unitsphere/mdl_pSphere1.xml");
	RenderController::GetInstance()->AddRenderContext( sphereModelPtr->GetShapes()[0]->GetRenderContext() );

	AabbModelPtr = new (EDRendererD3D::RenderModel)("GDModel/unitcube/mdl_pCube1.xml");
	RenderController::GetInstance()->AddRenderContext( AabbModelPtr->GetShapes()[0]->GetRenderContext() );

	coneModelPtr = new (EDRendererD3D::RenderModel)("GDModel/unitcone/mdl_pCone1.xml");
	//RenderController::GetInstance()->AddRenderContext( AabbModelPtr->GetShapes()[0]->GetRenderContext() );

	lineModelPtr = new (EDRendererD3D::RenderModel)("GDModel/mdl_DebugPlaceHolder.xml");
	lineModelPtr->GetShapes()[0]->RenderFunc = RenderShapeDebugLines::RenderDebugLines;
	//RenderController::GetInstance()->AddRenderContext( AabbModelPtr->GetShapes()[0]->GetRenderContext() );

	ID3DXBuffer *errors;
	D3DXCreateEffectFromFile(Renderer::theDevicePtr, L"DebugLine.fx", 0, 0, 
		0, Renderer::theEffectPoolPtr, &effectPtr, &errors);
#ifdef _DEBUG
	if (errors)
	{
		InternalOutput::GetReference() << (char *)errors->GetBufferPointer();
	}
#endif
}

DebugRenderer::~DebugRenderer()
{
	if(effectPtr)
	{
		effectPtr->Release();
		effectPtr = 0;
	}

	delete sphereModelPtr;
	delete AabbModelPtr;
	delete coneModelPtr;
	delete lineModelPtr;

	while( !oldList.empty() )
	{
		delete oldList.front();
		oldList.pop_front();
	}

	while( !renderList.empty() )
	{
		delete renderList.front();
		renderList.pop_front();
	}

	while( !sphereList.active.empty() )
	{
		delete sphereList.active.front();
		sphereList.active.pop_front();
	}

	while( !sphereList.inactive.empty() )
	{
		delete sphereList.inactive.front();
		sphereList.inactive.pop_front();
	}

	while( !aabbList.active.empty() )
	{
		delete aabbList.active.front();
		aabbList.active.pop_front();
	}

	while( !aabbList.inactive.empty() )
	{
		delete aabbList.inactive.front();
		aabbList.inactive.pop_front();
	}
}

DebugRenderer* DebugRenderer::GetInstance(void)
{
	if( instancePtr == 0 )
		instancePtr = new DebugRenderer;

	return instancePtr;
}

void DebugRenderer::DeleteInstance(void)
{
	delete instancePtr;
	instancePtr = 0;
}

void DebugRenderer::DrawSphere(const EDCollision::Sphere& sphere, const float3* colorPtr)
{
	EDRendererD3D::RenderModel* pModel = 0;

	if( sphereList.inactive.empty() )
	{
		pModel = new (EDRendererD3D::RenderModel)("GDModel/unitsphere/mdl_pSphere1.xml");
	}
	else
	{
		pModel = sphereList.inactive.front();
		sphereList.inactive.pop_front();	
	}

	float4x4 mat;
	mat.makeIdentity();

	mat.makeIdentity();
	mat.scale( sphere.radius );
	mat.WAxis = sphere.center;

	pModel->SetWorldMatrix(mat);
	sphereList.active.push_back(pModel);
}

void DebugRenderer::DrawAabb(const EDCollision::Aabb& aabb, const float3* colorPtr)
{
	//EDRendererD3D::RenderModel* pModel = 0;

	//if( aabbList.inactive.empty() )
	//{
	//	pModel = new (EDRendererD3D::RenderModel)("GDModel/unitcube/mdl_pCube1.xml");
	//}
	//else
	//{
	//	pModel = aabbList.inactive.front();
	//	aabbList.inactive.pop_front();	
	//}

	//float3 toC = (aabb.max - aabb.min) * 0.5f;
	//float4x4 mat;
	//mat.makeIdentity();
	//mat.WAxis = aabb.min + toC;
	//mat.XAxis *= toC.x;
	//mat.YAxis *= toC.y;
	//mat.ZAxis *= toC.z;

	//pModel->SetWorldMatrix(mat);
	//aabbList.active.push_back(pModel);

	float3 points[8];

	points[0] = aabb.min;
	points[7] = aabb.max;

	points[1] = float3( points[0].x, points[0].y, points[7].z );
	points[2] = float3( points[0].x, points[7].y, points[0].z );
	points[3] = float3( points[0].x, points[7].y, points[7].z );

	points[4] = float3( points[7].x, points[0].y, points[0].z );
	points[5] = float3( points[7].x, points[0].y, points[7].z );
	points[6] = float3( points[7].x, points[7].y, points[0].z );

	D3DXVECTOR4 color(1.0f, 1.0f, 1.0f, 1.0f);
	if( colorPtr != 0 )
	{
		color.x = colorPtr->x;
		color.y = colorPtr->y;
		color.z = colorPtr->z;
	}

	DrawSegment( points[0], points[1], color);	
	DrawSegment( points[1], points[3], color);	
	DrawSegment( points[3], points[2], color);	
	DrawSegment( points[2], points[0], color);	

	DrawSegment( points[4], points[5], color);	
	DrawSegment( points[5], points[7], color);	
	DrawSegment( points[7], points[6], color);	
	DrawSegment( points[6], points[4], color);	

	DrawSegment( points[0], points[4], color);	
	DrawSegment( points[1], points[5], color);	
	DrawSegment( points[2], points[6], color);	
	DrawSegment( points[3], points[7], color);	
}

void DebugRenderer::DrawCone(const float4x4 &matrix, const float3* colorPtr)
{
	EDRendererD3D::RenderModel* pModel = 0;

	if( coneList.inactive.empty() )
	{
		pModel = new (EDRendererD3D::RenderModel)("GDModel/unitcone/mdl_pCone1.xml");
	}
	else
	{
		pModel = coneList.inactive.front();
		coneList.inactive.pop_front();	
	}

	pModel->SetWorldMatrix((D3DXMATRIX *)&matrix);

	coneList.active.push_back(pModel);
}

void DebugRenderer::DrawSegment(const EDCollision::Segment& segment, 
	const D3DXVECTOR4& color)
{
	DrawSegment(segment.start, segment.end, color);
}

void DebugRenderer::DrawTriangle(const EDCollision::Triangle& triangle, const D3DXVECTOR4& color)
{
	float3 centroid(0.0f, 0.0f, 0.0f);

	for(unsigned int i = 0; i < 3; ++i)
	{
		DrawSegment( triangle.GetVertex(i), triangle.GetVertex( (1<<i) & 0x3 ) );
		centroid += triangle.GetVertex(i);
	}

	centroid *= (1.0f/3.0f);
	DrawSegment( centroid, centroid + triangle.GetNormal(), color );
}

void DebugRenderer::DrawSegment(const float3& p1, const float3& p2, 
	const D3DXVECTOR4& color)
{
	DrawSegment(*(D3DXVECTOR3 *)&p1, *(D3DXVECTOR3 *)&p2, color);
}

void DebugRenderer::DrawSegment(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR4& color)
{
	D3DXVECTOR3 points[2];
	points[0] = p1;
	points[1] = p2;
	DrawSegments(points, 2, color);
}

void DebugRenderer::DrawSegments(const D3DXVECTOR3 *points, UINT numPoints, 
	const D3DXVECTOR4& color)
{
	DrawLines(points, numPoints/2, color);
}

void DebugRenderer::DrawLines(const D3DXVECTOR3 *points, UINT numLines, 
	const D3DXVECTOR4& color)
{
	VERTEX_POSCOLOR *inPoints = new VERTEX_POSCOLOR[numLines*2];
	for(UINT i = 0; i < numLines*2; ++i)
	{
		inPoints[i].position = points[i];
		inPoints[i].color = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
	}

	RenderShapeDebugLines *shape = (RenderShapeDebugLines *)lineModelPtr->GetShapes()[0];
	shape->AddLines(inPoints, numLines);

	delete [] inPoints;
}

void DebugRenderer::DrawSegment(const VERTEX_POSCOLOR& p1, 
	const EDRendererD3D::VERTEX_POSCOLOR& p2)
{
	VERTEX_POSCOLOR points[2];
	points[0] = p1;
	points[1] = p2;
	DrawSegments(points, 2);
}

void DebugRenderer::DrawSegments(const VERTEX_POSCOLOR *points, 
	UINT numPoints)
{
	DrawLines(points, numPoints/2);
}

void DebugRenderer::DrawLines(const VERTEX_POSCOLOR *points, UINT numLines)
{
	RenderShapeDebugLines *shape = (RenderShapeDebugLines *)lineModelPtr->GetShapes()[0];
	shape->AddLines(points, numLines);
}

void DebugRenderer::DrawFrustum(const EDCollision::FrustumHull& frustum,
	const D3DXVECTOR4& color)
{
	DrawSegment( frustum.points[0], frustum.points[1], color );
	DrawSegment( frustum.points[1], frustum.points[3], color );
	DrawSegment( frustum.points[3], frustum.points[2], color );
	DrawSegment( frustum.points[2], frustum.points[0], color );

	DrawSegment( frustum.points[4], frustum.points[5], color );
	DrawSegment( frustum.points[5], frustum.points[7], color );
	DrawSegment( frustum.points[7], frustum.points[6], color );
	DrawSegment( frustum.points[6], frustum.points[4], color );

	DrawSegment( frustum.points[0], frustum.points[4], color );
	DrawSegment( frustum.points[1], frustum.points[5], color );
	DrawSegment( frustum.points[2], frustum.points[6], color );
	DrawSegment( frustum.points[3], frustum.points[7], color );	
}

void DebugRenderer::RenderDebugPrimitives(void)
{
	lineModelPtr->AddToContextSets();

	while( !oldList.empty() )
	{
		delete oldList.front();
		oldList.pop_front();
	}

	std::list< EDRendererD3D::RenderModel* >::iterator iter = renderList.begin();
	for(; iter != renderList.end(); ++iter)
	{
		(*iter)->AddToContextSets();
		oldList.push_back( (*iter) );
	}

	iter = sphereList.active.begin();
	for(; iter != sphereList.active.end(); ++iter)
	{
		(*iter)->AddToContextSets();
		sphereList.inactive.push_back( (*iter) );
	}
	sphereList.active.clear();

	iter = aabbList.active.begin();
	for(; iter != aabbList.active.end(); ++iter)
	{
		(*iter)->AddToContextSets();
		aabbList.inactive.push_back( (*iter) );
	}
	aabbList.active.clear();

	iter = coneList.active.begin();
	for(; iter != coneList.active.end(); ++iter)
	{
		(*iter)->AddToContextSets();
		coneList.inactive.push_back( (*iter) );
	}
	coneList.active.clear();

	renderList.clear();

//	lineModelPtr->AddToContextSets();

}

