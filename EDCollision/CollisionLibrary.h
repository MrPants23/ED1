#pragma once

#include "CollidableTypes.h"

#ifndef IndexFromPair
#define IndexFromPair(a,b) ( (a)<(b) ? 0 : 1 )
#endif

namespace EDCollision
{
    typedef bool(*CollisionFuncPointer)( CollisionFeedback *feedbackPtr);

    class CollisionLibrary
    {
    private:

        CollisionLibrary(const CollisionLibrary&){}
        CollisionLibrary& operator=(const CollisionLibrary &){ return *this; }
        CollisionLibrary(void);

        unsigned int offsetTable[EDCO_TYPE_COUNT];
        std::vector<CollisionFuncPointer> collisionFuncs;

        static bool DefaultNoCollisionFunc(CollisionFeedback *feedbackPtr){return false;}


        template<typename CLASS_A, typename CLASS_B, bool f(CLASS_A*, CLASS_B*, EDUtilities::float3&, EDUtilities::float3&)>
        static bool CollisionFunc(CollisionFeedback *feedbackPtr)
        {
            if( CLASS_A::CO_TYPE < CLASS_B::CO_TYPE )
            {
                return f( (CLASS_A*)feedbackPtr->collisionPair.collidablePtrs[0],
                    (CLASS_B*)feedbackPtr->collisionPair.collidablePtrs[1],
                    feedbackPtr->collisionPair.closestPoints[0],
                    feedbackPtr->collisionPair.closestPoints[1] );
            }
            else
            {
                return f( (CLASS_A*)feedbackPtr->collisionPair.collidablePtrs[1],
                    (CLASS_B*)feedbackPtr->collisionPair.collidablePtrs[0],
                    feedbackPtr->collisionPair.closestPoints[1],
                    feedbackPtr->collisionPair.closestPoints[0] );			
            }
        }

    public:
        static CollisionLibrary& GetInstance(void)
        {
            static CollisionLibrary instance;
            return instance;
        }

        CollisionFuncPointer GetPairCollisionFunc(unsigned int typeA, unsigned int typeB)
        {		
            return ( typeA > typeB ? collisionFuncs[ offsetTable[typeA] + typeB ] : collisionFuncs[ offsetTable[typeB] + typeA ] );
        }

        void SetPairCollisionFunc(unsigned int typeA, unsigned int typeB, CollisionFuncPointer func)
        {
            if( typeA < typeB )
            {
                unsigned int temp = typeA;
                typeA = typeB;
                typeB = temp;
            }

            collisionFuncs[ offsetTable[typeA] + typeB ] = func;	
        }

        template<typename CLASS_A, typename CLASS_B, bool f(CLASS_A*, CLASS_B*, EDUtilities::float3&, EDUtilities::float3&)>
        void SetPairCollisionFunc(void)
        {
            if( CLASS_A::CO_TYPE < CLASS_B::CO_TYPE )
                collisionFuncs[ offsetTable[CLASS_B::CO_TYPE] + CLASS_A::CO_TYPE ] = CollisionFunc<CLASS_A, CLASS_B, f>;
            else
                collisionFuncs[ offsetTable[CLASS_A::CO_TYPE] + CLASS_B::CO_TYPE ] = CollisionFunc<CLASS_A, CLASS_B, f>;
        }

        template<typename CLASS_A, typename CLASS_B, void f(CLASS_A*, CLASS_B*, EDUtilities::float3&, EDUtilities::float3&, unsigned int&)>
        static void CollisionReaction(CollisionFeedback *feedbackPtr)
        {
            if( CLASS_A::CO_TYPE < CLASS_B::CO_TYPE )
            {
                f( (CLASS_A*)feedbackPtr->collisionPair.collidablePtrs[0], 
                    (CLASS_B*)feedbackPtr->collisionPair.collidablePtrs[1],
                    feedbackPtr->collisionPair.closestPoints[0],
                    feedbackPtr->collisionPair.closestPoints[1],
                    feedbackPtr->resultFlags );
            }
            else
            {
                f( (CLASS_A*)feedbackPtr->collisionPair.collidablePtrs[1], 
                    (CLASS_B*)feedbackPtr->collisionPair.collidablePtrs[0],
                    feedbackPtr->collisionPair.closestPoints[1],
                    feedbackPtr->collisionPair.closestPoints[0],
                    feedbackPtr->resultFlags );
            }
        }

        static bool SphereTriangleCollide( Sphere* spherePtr, Triangle *trianglePtr, EDUtilities::float3& cpSphere, EDUtilities::float3& cpTriangle );
        static bool CapsuleTriangleCollide( Capsule* capsulePtr, Triangle *trianglePtr, EDUtilities::float3& cpCapsule, EDUtilities::float3 &cpTriangle );
        static bool FrustumHullSphereCollide( FrustumHull* frustumHullPtr, Sphere* spherePtr, EDUtilities::float3& cpFrustumHull, EDUtilities::float3 &cpSphere );
        static bool FrustumHullAabbCollide( FrustumHull* frustumHullPtr, Aabb* spherePtr, EDUtilities::float3& cpFrustumHull, EDUtilities::float3 &cpAabb );
        static bool SphereAabbCollide( Sphere* spherePtr, Aabb* aabbPtr, EDUtilities::float3& cpSphere, EDUtilities::float3& cpAabb );
        static bool SegmentTriangleCollide( Segment* segmentPtr, Triangle* trianglePtr, EDUtilities::float3& cpSegment, EDUtilities::float3& cpTriangle );
        static bool SegmentSphereCollide( Segment* segmentPtr, Sphere* spherePtr, EDUtilities::float3& cpSegment, EDUtilities::float3& cpSphere );
        static bool SphereSphereCollide( Sphere* sphereAPtr, Sphere* sphereBPtr, EDUtilities::float3& cpSphereA, EDUtilities::float3& cpSphereB );
        static bool SphereCapsuleCollide( Sphere* spherePtr, Capsule* capsulePtr, EDUtilities::float3& cpSphere, EDUtilities::float3& cpCapsule );
        static bool CollidableMeshCollide( CollisionFeedback *feedbackPtr );
        static bool SegmentAabbCollide( Segment* segmentPtr, Aabb* aabbPtr, EDUtilities::float3& cpSegment, EDUtilities::float3& cpAabb );
        static bool CapsuleAabbCollide( Capsule* capsulePtr, Aabb* aabbPtr, EDUtilities::float3& cpCapsule, EDUtilities::float3& cpAabb );
        static bool AabbAabbCollide( Aabb* aabb1Ptr, Aabb *aabb2Ptr, EDUtilities::float3 &cpAabb1, EDUtilities::float3 &cpAabb2 ); 

        static void CapsuleTriangleReaction( Capsule *capsulePtr, Triangle *trianglePtr, EDUtilities::float3& cpCapsule, EDUtilities::float3& cpTriangle, unsigned int &resultFlags );
        static void SphereTriangleReaction(Sphere* spherePtr, Triangle* trianglePtr, EDUtilities::float3 &cpSphere, EDUtilities::float3 &cpTriangle, unsigned int& resultFlags);
        static void SegmentTriangleClampReaction(Segment* segmentPtr, Triangle* trianglePtr, EDUtilities::float3& cpSegment, EDUtilities::float3& cpTriangle, unsigned int &resultFlags);
    };
}