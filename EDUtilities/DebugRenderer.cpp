#include "DebugRenderer.h"
#include "InternalOutput.h"
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
	//m_pSphereModel = MMNew(EDRendererD3D::RenderModel)("GDModel/unitsphere/mdl_pSphere1.xml");
	m_pSphereModel = new (EDRendererD3D::RenderModel)("GDModel/unitsphere/mdl_pSphere1.xml");
	RenderController::GetInstance()->AddRenderContext( m_pSphereModel->GetShapes()[0]->GetRenderContext() );

	//m_pAabbModel = MMNew(EDRendererD3D::RenderModel)("GDModel/unitcube/mdl_pCube1.xml");
	m_pAabbModel = new (EDRendererD3D::RenderModel)("GDModel/unitcube/mdl_pCube1.xml");
	RenderController::GetInstance()->AddRenderContext( m_pAabbModel->GetShapes()[0]->GetRenderContext() );

	//m_pConeModel = MMNew(EDRendererD3D::RenderModel)("GDModel/unitcone/mdl_pCone1.xml");
	m_pConeModel = new (EDRendererD3D::RenderModel)("GDModel/unitcone/mdl_pCone1.xml");
	//RenderController::GetInstance()->AddRenderContext( m_pAabbModel->GetShapes()[0]->GetRenderContext() );

	lineModelPtr = new (EDRendererD3D::RenderModel)("GDModel/mdl_DebugPlaceHolder.xml");
	lineModelPtr->GetShapes()[0]->RenderFunc = RenderShapeDebugLines::RenderDebugLines;
	//RenderController::GetInstance()->AddRenderContext( m_pAabbModel->GetShapes()[0]->GetRenderContext() );

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

	//MMDel(m_pSphereModel);
	//MMDel(m_pAabbModel);
	//MMDel(m_pConeModel);

	delete m_pSphereModel;
	delete m_pAabbModel;
	delete m_pConeModel;
	delete lineModelPtr;

	while( !m_OldList.empty() )
	{
		//MMDel( m_OldList.front() );
		delete m_OldList.front();
		m_OldList.pop_front();
	}

	while( !m_RenderList.empty() )
	{
		//MMDel( m_RenderList.front() );
		delete m_RenderList.front();
		m_RenderList.pop_front();
	}

	while( !m_SphereList.m_Active.empty() )
	{
		//MMDel( m_SphereList.m_Active.front() );
		delete m_SphereList.m_Active.front();
		m_SphereList.m_Active.pop_front();
	}

	while( !m_SphereList.m_Inactive.empty() )
	{
		//MMDel( m_SphereList.m_Inactive.front() );
		delete m_SphereList.m_Inactive.front();
		m_SphereList.m_Inactive.pop_front();
	}

	while( !m_AabbList.m_Active.empty() )
	{
		//MMDel( m_AabbList.m_Active.front() );
		delete m_AabbList.m_Active.front();
		m_AabbList.m_Active.pop_front();
	}

	while( !m_AabbList.m_Inactive.empty() )
	{
		//MMDel( m_AabbList.m_Inactive.front() );
		delete m_AabbList.m_Inactive.front();
		m_AabbList.m_Inactive.pop_front();
	}
}

DebugRenderer* DebugRenderer::GetInstance(void)
{
	if( instancePtr == 0 )
		instancePtr = new DebugRenderer;
		//instancePtr = MMNew(DebugRenderer);

	return instancePtr;
}

void DebugRenderer::DeleteInstance(void)
{
	//MMDel(instancePtr);
	delete instancePtr;
	instancePtr = 0;
}

void DebugRenderer::DrawSphere(const EDCollision::Sphere& sphere, const float3* pColor)
{
	EDRendererD3D::RenderModel* pModel = 0;

	if( m_SphereList.m_Inactive.empty() )
	{
		//pModel = MMNew(EDRendererD3D::RenderModel)("GDModel/unitsphere/mdl_pSphere1.xml");
		pModel = new (EDRendererD3D::RenderModel)("GDModel/unitsphere/mdl_pSphere1.xml");
	}
	else
	{
		pModel = m_SphereList.m_Inactive.front();
		m_SphereList.m_Inactive.pop_front();	
	}

	float4x4 mat;
	mat.makeIdentity();

	mat.makeIdentity();
	mat.scale( sphere.radius );
	mat.WAxis = sphere.center;

	pModel->SetWorldMatrix(mat);
	m_SphereList.m_Active.push_back(pModel);
}

void DebugRenderer::DrawAabb(const EDCollision::Aabb& aabb, const float3* pColor)
{
	//EDRendererD3D::RenderModel* pModel = 0;

	//if( m_AabbList.m_Inactive.empty() )
	//{
	//	pModel = new (EDRendererD3D::RenderModel)("GDModel/unitcube/mdl_pCube1.xml");
	//}
	//else
	//{
	//	pModel = m_AabbList.m_Inactive.front();
	//	m_AabbList.m_Inactive.pop_front();	
	//}

	//float3 toC = (aabb.max - aabb.min) * 0.5f;
	//float4x4 mat;
	//mat.makeIdentity();
	//mat.WAxis = aabb.min + toC;
	//mat.XAxis *= toC.x;
	//mat.YAxis *= toC.y;
	//mat.ZAxis *= toC.z;

	//pModel->SetWorldMatrix(mat);
	//m_AabbList.m_Active.push_back(pModel);

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
	if( pColor != 0 )
	{
		color.x = pColor->x;
		color.y = pColor->y;
		color.z = pColor->z;
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

void DebugRenderer::DrawCone(const float4x4 &matrix, const float3* pColor)
{
	EDRendererD3D::RenderModel* pModel = 0;

	if( m_ConeList.m_Inactive.empty() )
	{
		//pModel = MMNew(EDRendererD3D::RenderModel)("GDModel/unitcone/mdl_pCone1.xml");
		pModel = new (EDRendererD3D::RenderModel)("GDModel/unitcone/mdl_pCone1.xml");
	}
	else
	{
		pModel = m_ConeList.m_Inactive.front();
		m_ConeList.m_Inactive.pop_front();	
	}

	pModel->SetWorldMatrix((D3DXMATRIX *)&matrix);

	m_ConeList.m_Active.push_back(pModel);
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

void DebugRenderer::DrawSegments(const D3DXVECTOR3 *points, UINT numpoints, 
	const D3DXVECTOR4& color)
{
	DrawLines(points, numpoints/2, color);
}

void DebugRenderer::DrawLines(const D3DXVECTOR3 *points, UINT primCount, 
	const D3DXVECTOR4& color)
{
	VERTEX_POSCOLOR *inPoints = new VERTEX_POSCOLOR[primCount*2];
	for(UINT i = 0; i < primCount*2; ++i)
	{
		inPoints[i].position = points[i];
		inPoints[i].color = D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
	}

	RenderShapeDebugLines *shape = (RenderShapeDebugLines *)lineModelPtr->GetShapes()[0];
	shape->AddLines(inPoints, primCount);

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
	UINT numpoints)
{
	DrawLines(points, numpoints/2);
}

void DebugRenderer::DrawLines(const VERTEX_POSCOLOR *points, UINT primCount)
{
	RenderShapeDebugLines *shape = (RenderShapeDebugLines *)lineModelPtr->GetShapes()[0];
	shape->AddLines(points, primCount);
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

	while( !m_OldList.empty() )
	{
		//MMDel( m_OldList.front() );
		delete m_OldList.front();
		m_OldList.pop_front();
	}

	std::list< EDRendererD3D::RenderModel* >::iterator iter = m_RenderList.begin();
	for(; iter != m_RenderList.end(); ++iter)
	{
		(*iter)->AddToContextSets();
		m_OldList.push_back( (*iter) );
	}

	iter = m_SphereList.m_Active.begin();
	for(; iter != m_SphereList.m_Active.end(); ++iter)
	{
		(*iter)->AddToContextSets();
		m_SphereList.m_Inactive.push_back( (*iter) );
	}
	m_SphereList.m_Active.clear();

	iter = m_AabbList.m_Active.begin();
	for(; iter != m_AabbList.m_Active.end(); ++iter)
	{
		(*iter)->AddToContextSets();
		m_AabbList.m_Inactive.push_back( (*iter) );
	}
	m_AabbList.m_Active.clear();

	iter = m_ConeList.m_Active.begin();
	for(; iter != m_ConeList.m_Active.end(); ++iter)
	{
		(*iter)->AddToContextSets();
		m_ConeList.m_Inactive.push_back( (*iter) );
	}
	m_ConeList.m_Active.clear();

	m_RenderList.clear();

//	lineModelPtr->AddToContextSets();

}

