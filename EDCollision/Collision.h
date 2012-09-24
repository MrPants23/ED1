#pragma once

#include "../EDUtilities/float3.h"
#include "CollisionPrimitives.h"
#include "CollidableTypes.h"

#ifndef FLT_EPSILON
#define FLT_EPSILON 1.192092896e-07F
#endif

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

#ifndef HalfSpaceTest
#define HalfSpaceTest(plane, testPoint) (DotProduct( (plane).normal, (testPoint) ) - (plane).offset)
#endif

#ifndef ED_EPSILON
#define ED_EPSILON 3.4526698307252027193509668742125e-04F
#endif

namespace EDCollision
{
    void ClosestPointLine(const float3 &pointA, const float3 &pointB,
        const float3 &testPoint, 
        float &time, float3 &closestPoint);

    void ClosestPointRay( const float3 &rayStart, const float3 &rayDir, 
        const float3 &testPoint, 
        float &time, float3 &closestPoint);

    void ClosestPointSegment( const float3 &pointA, const float3 &pointB, 
        const float3 &testPoint, float &time, 
        float3 &closestPoint );

    void ClosestPointAABB( const float3 &boxMin, const float3 &boxMax, 
        const float3 &testPoint, float3 &closestPoint );

    bool LineSphereCollide( const float3 &pointA, const float3 &pointB, 
        const float3 &sphereCenter, float radius );

    bool RaySphereCollide( const float3 &rayStart, const float3 &rayDir, 
        const float3 &sphereCenter, float radius );

    bool SegmentSphereCollide( const float3 &pointA, const float3 &pointB, 
        const float3 &sphereCenter, float radius );

    bool AABBAABBCollide(const float3 &boxAMin, const float3 &boxAMax, 
        const float3 &boxBMin, const float3 &boxBMax);

    bool SphereSphereCollide(const float3 &sphereACenter, float radiusA, 
        const float3 &sphereCenterB, float radiusB);

    bool SphereAABBCollide(const float3 &sphereCenter, float radius, 
        const float3 &boxMin, const float3 &boxMax);

    bool SpherePlaneCollide( const float3 &sphereCenter, float radius, 
        const float3 &planeNormal, float planeOffset );

    bool LinePlaneIntersect( const float3 &pointA, const float3 &pointB, 
        const float3 planeNormal, float planeOffset, float &time );

    bool SegmentPlaneIntersect( const float3 &pointA, const float3 &pointB, 
        const float3 &planeNormal, float planeOffset,
        float &time );

    bool RayPlaneIntersect( const float3 &rayStart, const float3 &rayDir, 
        const float3 &planeNormal, float planeOffset, 
        float &time );

    bool LineTriangleIntersect( const float3 &vertA, const float3 &vertB, 
        const float3 &vertC, const float3 &triNorm, 
        const float3 &pointA, const float3 &pointB, 
        float &time );

    bool SegmentTriangleIntersect( const float3 &vertA, const float3 &vertB,
        const float3 &vertC, const float3 &triNorm,
        const float3 &pointA, const float3 &pointB,
        float &time );

    bool RayTriangleIntersect( const float3 &vertA, const float3 &vertB, 
        const float3 &vertC, const float3 &triNorm, 
        const float3 &rayStart, const float3 &rayDir, 
        float &time );

    bool AABBSegmentCollide(const float3 &pointA, const float3 &pointB, 
        const float3 &boxMin, const float3 &boxMax);

    // Closest point tests
    void ClosestPointAABB( const Aabb &box, const float3 &testPoint, 
        float3 &closestPoint );

    // Linear primitives to sphere collision
    bool LineSphereCollide( const float3 &pointA, const float3 &pointB, 
        const Sphere &sphere );

    bool RaySphereCollide( const float3 &rayStart, const float3 &rayDir, 
        const Sphere &sphere );

    bool SegmentSphereCollide( const float3 &pointA, const float3 &pointB, 
        const Sphere &sphere );

    // AABB to AABB collide
    bool AABBAABBCollide(const Aabb &boxA, const Aabb &boxB);

    // Sphere to Sphere collide
    bool SphereSphereCollide(const Sphere &sphereA, 
        const Sphere &sphereB);

    // Sphere to AABB collide
    bool SphereAABBCollide( const Sphere &sphere, 
        const Aabb &box );

    // Sphere to plane collide
    bool SpherePlaneCollide( const Sphere &sphere, 
        const Plane &plane );

    // Linear primitives to plane intersect
    bool LinePlaneIntersect( const float3 &pointA, const float3 &pointB, 
        const Plane &plane, float &time );

    bool SegmentPlaneIntersect( const float3 &pointA, const float3 &pointB, 
        const Plane &plane, float &time );

    bool RayPlaneIntersect( const float3 &rayStart, const float3 &rayDir, 
        const Plane &plane, float &time );

    void BuildSphere( Sphere &sphere, const float *verts, unsigned int numVerts );
}