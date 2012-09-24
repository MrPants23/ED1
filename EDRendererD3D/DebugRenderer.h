#pragma once

#include "../EDUtilities/float3.h"
#include "../EDUtilities/float4x4.h"
#include <list>
using namespace EDUtilities;
#include "../EDRendererD3D/VertexFormats.h"

using namespace EDRendererD3D;

namespace EDRendererD3D
{
	class RenderModel;
	class RenderShapeDebugLines;
};

namespace EDCollision
{
	class Sphere;
	class Aabb;
	class Segment;
	class Triangle;
	class FrustumHull;
}

struct PrimitiveList
{
	std::list< EDRendererD3D::RenderModel* > active;
	std::list< EDRendererD3D::RenderModel* > inactive;
};

class DebugRenderer
{	
private:
	RenderModel* sphereModelPtr;
	RenderModel* AabbModelPtr;
	RenderModel *coneModelPtr;
	RenderModel *lineModelPtr;

	std::list< EDRendererD3D::RenderModel* > renderList;
	std::list< EDRendererD3D::RenderModel* > oldList;

	PrimitiveList sphereList;
	PrimitiveList aabbList;
	PrimitiveList coneList;

	ID3DXEffect	*effectPtr;

	DebugRenderer(void);
	DebugRenderer(const DebugRenderer&){}
	DebugRenderer& operator=(const DebugRenderer&){ return *this; }

	static DebugRenderer* instancePtr;
	/// Draws a list of lines.
	/// This is a helper method to group similar functionality of 
	/// drawing debug lines.
	/// \param points - A list of points to draw lines between
	/// \param primCount - The number of lines to render
	/// \param color - The color to draw the lines
	void DrawLines(const D3DXVECTOR3 *points, UINT numLines, 
		const D3DXVECTOR4& color);

	/// Draws a list of lines with vertex colors.
	/// This is a helper method to group similar functionality of 
	/// drawing debug lines.
	/// \param points - A list of points to draw lines between
	/// \param primCount - The number of lines to render
	/// \param color - The color to draw the lines
	void DrawLines(const EDRendererD3D::VERTEX_POSCOLOR *points, UINT numLines);

public:
	
	~DebugRenderer(void);

	void DrawSphere(const EDCollision::Sphere& sphere, const float3* colorPtr = 0);
	void DrawAabb(const EDCollision::Aabb& aabb, const float3* colorPtr = 0);
	void DrawCone(const float4x4 &matrix, const float3* colorPtr = 0);

	/// Draws a single line
	/// \param segment - The segement to draw
	/// \param color - The color to draw the line
	void DrawSegment(const EDCollision::Segment& segment,
		const D3DXVECTOR4& color = D3DXVECTOR4(1, 1, 1, 1));
	/// Draws a single line
	/// \param p1 - The first point of the line
	/// \param p2 - The first point of the line
	/// \param color - The color to draw the line
	void DrawSegment(const float3& p1, const float3& p2, 
		const D3DXVECTOR4& color = D3DXVECTOR4(1, 1, 1, 1));
	/// Draws a single line
	/// \param p1 - The first point of the line
	/// \param p2 - The first point of the line
	/// \param color - The color to draw the line
	void DrawSegment(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, 
		const D3DXVECTOR4& color = D3DXVECTOR4(1, 1, 1, 1));

	/// Draws a list of lines
	/// \param points - A list of points to draw lines between
	/// \param numPoints - The number of points contained in varaible points
	/// \param color - The color to draw the lines
	void DrawSegments(const D3DXVECTOR3 *points, UINT numPoints, 
		const D3DXVECTOR4& color = D3DXVECTOR4(1, 1, 1, 1));

	/// Draws a single line with vertex colors
	/// \param p1 - The first point of the line
	/// \param p2 - The first point of the line
	void DrawSegment(const EDRendererD3D::VERTEX_POSCOLOR& p1, 
		const EDRendererD3D::VERTEX_POSCOLOR& p2);

	/// Draws a list of lines with vertex colors
	/// \param points - A list of points to draw lines between
	/// \param numPoints - The number of points contained in varaible points
	void DrawSegments(const EDRendererD3D::VERTEX_POSCOLOR *points, 
		UINT numPoints);

	void DrawTriangle(const EDCollision::Triangle& triangle, 
		const D3DXVECTOR4& color = D3DXVECTOR4(1, 1, 1, 1));
	void DrawFrustum(const EDCollision::FrustumHull& frustum, 
		const D3DXVECTOR4& color = D3DXVECTOR4(1, 1, 1, 1));

	void RenderDebugPrimitives(void);

	static DebugRenderer* GetInstance(void);
	static void DeleteInstance(void);
};