#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDCollision/CollidableTypes.h"
#include "../EDTree/AabbBvh.h"
#include "../EDTree/AabbBvhLeafNode.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDUtilities/GDAttribute.h"
#include "../EDUtilities/GDAttributeTypes.h"
#include "CollisionContact.h"

using namespace EDCollision;
using namespace EDUtilities;
using namespace EDGOCA;

namespace Behaviors
{
	struct BVHTraversalStruct;
	typedef bool (*BVHPerNodeAabbFunc)(const Aabb& nodeAabb, BVHTraversalStruct &traversalStruct);
	typedef void (*BVHPerTriangleFunc)(const float3& v0, const float3& v1, const float3& v2, const float3& n, BVHTraversalStruct &traversalStruct);

	struct BVHTraversalStruct
	{
		BVHPerNodeAabbFunc m_PerNodeFunc;
		BVHPerTriangleFunc m_PerTriangleFunc;
		void* m_pData;

		BVHTraversalStruct(void)
		{
			m_PerNodeFunc = 0;
			m_PerTriangleFunc = 0;
			m_pData = 0;
		}
	};

	class BVHTemplate
	{
		AabbBvh* m_pAabbBvh;
		ContentHandle<GDAttribute<Point>> m_PointsHandle;
		ContentHandle<GDAttribute<TriVertIndices>> m_IndicesHandle;
		ContentHandle<GDAttribute<TriNormal>> m_TriNormalsHandle;
		
		void Traverse(const EDTreeNode* node, BVHTraversalStruct& traversalStruct);

	public:
		
		BVHTemplate(void) : m_pAabbBvh(0) {}
		~BVHTemplate() { delete m_pAabbBvh; }

		static BVHTemplate* Load(const char* pGDMeshFile, const char* indentifier = 0);

		void Traverse( BVHTraversalStruct& traversalStruct );

		void GetLocalAabb(Aabb& aabb)const;
	};

	class Collider : public BehaviorT<Collider>
	{
	public:
		enum ColliderType{ INVALID = -1, AABB = 0, OBB, SPHERE, CAPSULE, MESH };

		class ColliderObject
		{
		public:
			virtual ~ColliderObject(){}
			virtual ColliderType GetColliderType(void) = 0;
			virtual void GetLocalAabb(Aabb& aabb) = 0;
			virtual void ReadXML(TiXmlElement* objectEle) = 0;
			virtual ColliderObject* clone(void) = 0;
		};

		class AabbColliderObject : public ColliderObject
		{
			Aabb localAabb;
		public:
			ColliderType GetColliderType(void){ return AABB; }
			void GetLocalAabb(Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);
			AabbColliderObject* clone(void) { return new AabbColliderObject(*this); }
		};

		class ObbColliderObject : public ColliderObject
		{
			Obb localObb;
		public:
			ColliderType GetColliderType(void){ return OBB; }
			void GetLocalAabb(Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);
			ObbColliderObject* clone(void) { return new ObbColliderObject(*this); }
		};

		class SphereColliderObject : public ColliderObject
		{
			Sphere localSphere;
		public:
			ColliderType GetColliderType(void){ return SPHERE; }
			void GetLocalAabb(Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);
			const Sphere& GetLocalSphere(void){ return localSphere; }
			SphereColliderObject* clone(void){ return new SphereColliderObject(*this); }
		};

		class CapsuleColliderObject : public ColliderObject
		{
			Capsule localCapsule;
		public:
			ColliderType GetColliderType(void){ return CAPSULE; }
			void GetLocalAabb(Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);
			CapsuleColliderObject* clone(void){ return new CapsuleColliderObject(*this); }
		};

		class MeshColliderObject : public ColliderObject
		{
			ContentHandle<BVHTemplate> bvhHandle;
		public:
			ColliderType GetColliderType(void){ return MESH; }
			void GetLocalAabb(Aabb& aabb);
			void ReadXML(TiXmlElement* objectEle);

			void Traverse(BVHTraversalStruct& traversalStruct);
			MeshColliderObject* clone(void){ return new MeshColliderObject(*this); }
		};
	
		static bool ColliderAabbToAabb(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderAabbToObb(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderAabbToSphere(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderAabbToCapsule(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderAabbToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact);

		static bool ColliderObbToObb(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderObbToSphere(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderObbToCapsule(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderObbToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		
		static bool ColliderSphereToSphere(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderSphereToCapsule(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderSphereToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact);

		static bool ColliderCapsuleToCapsule(Collider* objA, Collider* objB, CollisionContact& collisionContact);
		static bool ColliderCapsuleToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact);

		static bool ColliderMeshToMesh(Collider* objA, Collider* objB, CollisionContact& collisionContact);

		Collider(void) : colliderObject(0) {}
		~Collider(void);

		ColliderType GetColliderType(void)const;

		const ColliderObject* GetColliderObject(void)const{ return colliderObject; }

		const AttributeNameKey* GetRequiredAttributes(void);

		static void PopulateMessageMap(void);
		static void OnCollide( IBehavior* invokingBehavior, IMessage* message );
		static void OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnGetCollider( IBehavior* invokingBehavior, IMessage* message );
		static void OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnIntersectSegment( IBehavior* pInvokingBehavior, IMessage* msg );

	private:

		ColliderObject* colliderObject;
	};
};