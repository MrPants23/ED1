#pragma once

#include "../EDUtilities/float3.h"
#include "../EDUtilities/float4x4.h"
using namespace EDUtilities;

#include <fstream>
#include <limits>
#include <vector>
using namespace std;

#ifndef FLT_EPSILON
#define FLT_EPSILON 1.192092896e-07F
#endif

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

#ifndef ED_EPSILON
#define ED_EPSILON 3.4526698307252027193509668742125e-04F
#endif

#define ED_TRIANGLES                      0x0004
#define ED_TRIANGLE_STRIP                 0x0005
#define ED_TRIANGLE_FAN                   0x0006

namespace EDCollision
{
    enum EDCO_Type{ EDCO_NULL = 0, EDCO_LINE, EDCO_RAY, EDCO_SEGMENT, EDCO_PLANE, EDCO_TRIANGLE, EDCO_TRIANGLE_XF, EDCO_SPHERE, EDCO_AABB, EDCO_OBB, EDCO_CAPSULE, EDCO_KDOP, EDCO_CONVEXHULL, EDCO_FRUSTUM, EDCO_MESH, EDCO_TYPE_COUNT };
    enum CollisionResults{ EDCR_NONE = 0x0, EDCR_COLLISION = 0x1, EDCR_NEW_COLLISION = 0x80000000 };

    class Line;
    class Ray;
    class Segment;
    class Plane;
    class Triangle;
    class Sphere;
    class Aabb;
    class Obb;
    class Capsule;
    class ConvexHull;
    class FrustumHull;

    class Collidable;
    struct CollisionPair
    {
        Collidable* collidablePtrs[2];
        EDUtilities::float3 closestPoints[2];
    };

    class CollisionFeedback
    {
    public:
        CollisionFeedback(void)
        {
            resultFlags = 0x0;
            collisionPair.collidablePtrs[0] = 0;
            collisionPair.collidablePtrs[1] = 0;
            collidablePtr = 0;
        }
        virtual ~CollisionFeedback(){}

        CollisionPair collisionPair;
        Collidable* collidablePtr;
        unsigned int resultFlags;
    };

    typedef void(*CollisionResponseFunc)( CollisionFeedback *feedbackPtr );

    class Collidable
    {
    public:
        Collidable(void){}
        virtual ~Collidable(void) = 0 {};

        virtual unsigned int GetCOType(void) const = 0 { return EDCO_NULL; }

        // 0 == back, 1 == front, 2 == straddling 
        virtual unsigned int ClassifyToPlane(const Plane& plane) const { return 2; }
        virtual unsigned int ClassifyToAAPlane(unsigned int axis, float offset);

        virtual void Transform(const float4x4 &transform){}
        void InverseTransform(const float4x4 &transform);

        bool Collides(Collidable* inputPtr, CollisionResponseFunc func = 0, CollisionFeedback *feedbackPtr = 0);
    };


    class BoundingVolume : public Collidable
    {
    public:
        BoundingVolume(void){}
        virtual ~BoundingVolume(){}

        virtual void Build(const float *pointsPtr, unsigned int nCount){}
        virtual void Build(const BoundingVolume *boundsAPtr, const BoundingVolume *boundsBPtr){}
        virtual void GetSplitAxis(const float *pointsPtr, unsigned int numPoints, EDUtilities::float3 &splitAxis, float &min, float &max);

        virtual const EDUtilities::float3 &GetCentroid(void) = 0;
        virtual float GetSquaredRadius(void) const = 0;
        virtual float GetRadius(void) const { return sqrt(GetSquaredRadius()); }
        virtual void Translate(const EDUtilities::float3& translation){}

        static void ReadBV(fstream &fin, BoundingVolume* &bvPtr);
        static void WriteBV(fstream &fout, BoundingVolume* bvPtr);
    };
}