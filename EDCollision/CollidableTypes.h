#pragma once

#include "Collidable.h"
#include <vector>

namespace EDCollision
{
    class Line : public Collidable
    {
    public:
        Line(void){}
        virtual ~Line(){}

        static const unsigned int CO_TYPE = EDCO_LINE;

        virtual unsigned int GetCOType(void)const { return EDCO_LINE; }


        EDUtilities::float3 pointA;
        EDUtilities::float3 pointB;
    };

    class Ray : public Collidable
    {
    public:
        Ray(void){}
        virtual ~Ray(){}

        static const unsigned int CO_TYPE = EDCO_RAY;

        virtual unsigned int GetCOType(void)const { return EDCO_RAY; }

        EDUtilities::float3 start;
        EDUtilities::float3 direction;
    };

    class Segment : public Collidable
    {
    public:
        Segment(void){}
        virtual ~Segment(){}

        static const unsigned int CO_TYPE = EDCO_SEGMENT;

        virtual unsigned int GetCOType(void)const { return EDCO_SEGMENT; }
        virtual void Transform(const EDUtilities::float4x4 &transform);

        virtual float ClosestPoint( const EDUtilities::float3 *point, CollisionFeedback *feedbackPtr = 0 );
        virtual float ClosestPoints( Segment *segmentPtr, CollisionFeedback *feedbackPtr = 0 );
        virtual float ClosestPoints( Segment *segmentPtr, EDUtilities::float3 &cpThis, EDUtilities::float3 &cpOther );
        virtual float ClosestPoints( Triangle *trianglePtr, EDUtilities::float3 &cpSegment, EDUtilities::float3 &cpTriangle );

        virtual bool IntersectTriangle(Triangle* trianglePtr, EDUtilities::float3 &intersection);
        virtual unsigned int ClassifyToPlane(const Plane& plane) const;

        EDUtilities::float3 start;
        EDUtilities::float3 end;
    };

    class Plane : public Collidable
    {
    public:
        Plane(void){}
        virtual ~Plane(){}

        static const unsigned int CO_TYPE = EDCO_PLANE;

        virtual unsigned int GetCOType(void)const { return EDCO_PLANE; }

        virtual void Transform(const EDUtilities::float4x4 &transform);

        EDUtilities::float3 normal;
        float offset;
    };

    class Sphere : public BoundingVolume
    {
    public:
        static const unsigned int CO_TYPE = EDCO_SPHERE;
        Sphere(void){}
        virtual ~Sphere(){}

        virtual unsigned int GetCOType(void)const { return EDCO_SPHERE; }

        virtual void Transform(const EDUtilities::float4x4 &transform);

        virtual void Build(const float *pointsPtr, unsigned int numPoints);
		virtual void Build(const float *pointsPtr, unsigned int numPoints, unsigned int offset);

        virtual void Build(const BoundingVolume *boundsAPtr, const BoundingVolume *boundsBPtr);
        void Build(const EDUtilities::float3 *mPoints, unsigned int numPoints);
        virtual unsigned int ClassifyToPlane(const Plane& plane) const;

        EDUtilities::float3 center;
        float radius;

        virtual const EDUtilities::float3 &GetCentroid(void){ return center; }
        virtual float GetSquaredRadius(void) const { return radius*radius; }
        virtual float GetRadius(void) const { return radius; }

        virtual void Translate(const EDUtilities::float3& translation)
        {
            center += translation;
        }
    };

    class Aabb : public BoundingVolume
    {
        EDUtilities::float3 centeroid;
    public:
        Aabb(){}
        Aabb(const EDUtilities::float3& _min, const EDUtilities::float3& _max) {min = _min; max = _max;}

        virtual ~Aabb(){}

        static const unsigned int CO_TYPE = EDCO_AABB;

        virtual unsigned int GetCOType(void)const { return CO_TYPE; }

        virtual void Build(const float *pointsPtr, unsigned int numPoints);
		virtual void Build(const float *pointsPtr, unsigned int numPoints, unsigned int offset);
		virtual void Build(const EDUtilities::float3* pointsPtr, const unsigned int* indicesPtr, unsigned int numIndices);
        virtual void Build(const BoundingVolume *boundsAPtr, const BoundingVolume *boundsBPtr);
        virtual unsigned int ClassifyToPlane(const Plane& plane) const;

        virtual unsigned int ClassifyToAAPlane(unsigned int axis, float offset)
        {
            if( max.v[axis] < offset )
                return 0;
            else if( min.v[axis] > offset )
                return 1;
            else
                return 2;
        }

        EDUtilities::float3 min;
        EDUtilities::float3 max;

        virtual const EDUtilities::float3 &GetCentroid(void)
        {
            centeroid = (min + max) * 0.5f;
            return centeroid;
        }

        virtual float GetSquaredRadius(void) const 
        {
            EDUtilities::float3 vToMax = (max - min) * 0.5f;
            return DotProduct( vToMax, vToMax );
        }

        virtual void Translate(const EDUtilities::float3& translation)
        {
            min += translation;
            max += translation;
        }
    };

    class Obb : public BoundingVolume
    {
    public:
        Obb(void){}
        virtual ~Obb(){}

        static const unsigned int CO_TYPE = EDCO_OBB;

        virtual unsigned int GetCOType(void)const { return CO_TYPE; }

        EDUtilities::float4x4 transform;
        EDUtilities::float3 extents;

        virtual const EDUtilities::float3 &GetCentroid(void){ return transform.WAxis; }

        float GetSquaredRadius(void) const 
        {
            return DotProduct( extents, extents );
        }

        virtual void Translate(const EDUtilities::float3& translation)
        {
            transform.WAxis += translation;
        }
    };

    class Capsule : public BoundingVolume
    {
        EDUtilities::float3 centeroid;
    public:
        Capsule(void){}
        virtual ~Capsule(void){}

        static const unsigned int CO_TYPE = EDCO_CAPSULE;

        virtual unsigned int GetCOType(void)const { return CO_TYPE; }

        virtual void Transform(const EDUtilities::float4x4 &transform);
        virtual unsigned int ClassifyToPlane(const Plane& plane) const;

        Segment segment;
        float radius;	

        virtual const EDUtilities::float3 &GetCentroid(void)
        {
            centeroid = (segment.start + segment.end) * 0.5f;
            return centeroid;
        }

        virtual float GetSquaredRadius(void) const 
        {
            return radius*radius;
        }

        virtual float GetRadius(void) const 
        {
            return radius;
        }

        virtual void Translate(const EDUtilities::float3& translation)
        {
            segment.start += translation;
            segment.end += translation;
        }
    };

    template<unsigned int K>
    class Kdop : public BoundingVolume
    {
        EDUtilities::float3 centeroid;
    public:
        Kdop(void){}
        virtual ~Kdop(){}

        static const unsigned int CO_TYPE = EDCO_KDOP;

        virtual unsigned int GetCOType(void)const { return CO_TYPE; }

        virtual const EDUtilities::float3 &GetCentroid(void)
        {
            //Todo...
            return centeroid;
        }	

        EDUtilities::float3 normals[K];
        std::pair<float, float> slabs[K];

        virtual float GetSquaredRadius(void) const 
        {
            //Todo...
            return 0.0f;
        }

        virtual void Translate(const EDUtilities::float3& translation)
        {
            for(unsigned int i = 0; i < K; ++i)
            {
                float offset = DotProduct( translation, normals[K] );
                slabs[i].first += offset;
                slabs[i].second += offset;
            }
        }
    };

    class ConvexHull : public BoundingVolume
    {
        EDUtilities::float3 centeroid;
    public:
        ConvexHull(void){}
        virtual ~ConvexHull(){}

        static const unsigned int CO_TYPE = EDCO_CONVEXHULL;

        virtual unsigned int GetCOType(void)const { return CO_TYPE; }

        std::vector< EDUtilities::float3 > points;
        std::vector< Plane > planes;

        virtual const EDUtilities::float3 &GetCentroid(void)
        {
            centeroid.makeZero();
			size_t numPoints = points.size();
            for(unsigned int i = 0; i < numPoints; ++i)
                centeroid += points[i];

            centeroid *= (1.0f / numPoints);
            return centeroid;
        }

        virtual float GetSquaredRadius(void) const 
        {
            //todo...
            return 0.0f;
        }

        virtual void Translate(const EDUtilities::float3& translation)
        {
			size_t numPlanes = planes.size();
            for(unsigned int i = 0; i < numPlanes; ++i)
                planes[i].offset += DotProduct( translation, planes[i].normal );

			size_t numPoints = points.size();
            for(size_t i = 0; i < numPoints; ++i)
                points[i] += translation;
        }
    };

    class Triangle : public Collidable
    {
    public:
        Triangle(void){}
        virtual ~Triangle(){}

        static const unsigned int CO_TYPE = EDCO_TRIANGLE;

        virtual unsigned int GetCOType(void)const { return CO_TYPE; }
        virtual bool TestIntersectSegment( Segment *segmentPtr, EDUtilities::float3 &intersection );

        unsigned int indices[3];
        const EDUtilities::float3 *normalPtr;
        const EDUtilities::float3 *vertsPtr;

        const EDUtilities::float3 &GetVertex(unsigned int index)const{ return vertsPtr[ indices[index] ]; }
        const EDUtilities::float3 &GetNormal(void)const{ return *normalPtr; }

        float ClosestPoints(Segment *segmentPtr, EDUtilities::float3 &cpTriangle, EDUtilities::float3 &cpSegment);
    };

	//class TriangleXF : public Triangle
	//{
	//public:
	//	const float4x4& transform;

	//	TriangleXF(const float4x4 &xf) : transform(xf) {}
	//	virtual ~TriangleXF(){}

	//	static const unsigned int CO_TYPE = EDCO_TRIANGLE_XF;

 //       virtual unsigned int GetCOType(void)const { return CO_TYPE; }
 //       virtual bool TestIntersectSegment( Segment *segmentPtr, EDUtilities::float3 &intersection );
	//	
 //       float ClosestPoints(Segment *segmentPtr, EDUtilities::float3 &cpTriangle, EDUtilities::float3 &cpSegment);
	//}

    class FrustumHull : public Collidable
    {
        static unsigned int renderFrameNumber;

    public:
        FrustumHull(void){}

        static const unsigned int CO_TYPE = EDCO_FRUSTUM;

        virtual ~FrustumHull(){}

        virtual unsigned int GetCOType(void)const { return CO_TYPE; }

        virtual void Transform(const EDUtilities::float4x4 &transform);

        // Near, far, left, right, top, bottom
        Plane planes[6];

        EDUtilities::float3 points[8];

        // 0 == back, 1 == front, 2 == straddling 
        unsigned int ClassifyToPlane(const Plane& plane) const
        {
            unsigned int frontCount = 0;
            unsigned int backCount = 0;

            for(unsigned int i = 0; i < 8; ++i)
            {
                if( DotProduct( points[i], plane.normal ) - plane.offset >= 0.0f )
                    ++frontCount;
                else
                    ++backCount;
            }

            if( frontCount > 0 && backCount > 0 )
                return 2;
            else if( frontCount > 0 )
                return 1;
            else
                return 0;
        }

        unsigned int GetRenderFrameNumber(void){ return renderFrameNumber; }
        void SetRenderFrameNumber(unsigned int num){ renderFrameNumber = num; }
        void IncrementRenderFrameNumber(void){ ++renderFrameNumber; }

		void Build(float fov, float width, float height, float nearDist, float farDist);
    };


    class MeshHull : public Collidable
    {
    public:
        typedef void (*TriangleFunction)(Triangle *trianglePtr, void *voidPtr);

        MeshHull(void){}
        virtual ~MeshHull(){}

        static const unsigned int CO_TYPE = EDCO_MESH;

        virtual unsigned int GetCOType(void)const { return CO_TYPE; }
        void ForEachTriangle( TriangleFunction func, void *voidPtr ) const;

        void Build(const float* pointsPtr, unsigned int numVerts/*, bool bIsStrip = false*/ );
        void Build(const float* pointsPtr, unsigned int numVerts, const unsigned int* indicesPtr, unsigned int numIndices/*, bool bIsStrip = false*/);

        unsigned int numTriangles;
        std::vector<unsigned int> indices;
        std::vector<EDUtilities::float3> verts;
        std::vector<EDUtilities::float3> triangleNormals;
    };

    class MeshHullCollisionFeedback : public CollisionFeedback
    {
    public:
        MeshHull::TriangleFunction perTriangleFunc;
        CollisionResponseFunc perTriangleResponseFunc;
        CollisionResponseFunc perMeshResponseFunc;

        MeshHullCollisionFeedback(void) : perTriangleFunc(PerTriangleCallback), perTriangleResponseFunc(0), perMeshResponseFunc(0){}

        static void PerTriangleCallback(Triangle *trianglePtr, void *voidPtr);
    };
}