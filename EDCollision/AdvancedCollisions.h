#pragma once

#include "Collision.h"

namespace EDCollision
{
    bool RaySphereIntersectAC( const float3 &rayStart, const float3 &rayDir, const float3 &sphereCenter, float radius, float &time );

    bool RayTriangleIntersectAC( const float3 &vert0, const float3 &vert1, const float3 &vert2, const float3 &norm, const float3 &rayStart, const float3 &rayDir, float &time );

    bool MovingSphereTriangleIntersectAC( const float3 &vert0, const float3 &vert1, const float3 &vert2, const float3 &triNorm, const float3 &sphereStart, const float3 &sphereDirNorm, float radius, float &time, float3 &surfaceNormal );

    bool RayCylinderIntersectAC(const float3 &rayStart, const float3 &rayNormal, const float3 &cylBasePt, const float3 &cylTopPt, float radius, float &time);

    bool RayCapsuleIntersectAC(const float3 &rayStart, const float3 &rayNormal, const float3 &capsuleBasePt, const float3 &capsuleTopPt, float radius, float &time);

}