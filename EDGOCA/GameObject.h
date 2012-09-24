#pragma once

#include <map>
#include <list>
#include "TransformNode.h"
#include "IBehavior.h"
#include "../EDCollision/CollidableTypes.h"
#include "../EDUtilities/ContentManager.h"

using namespace EDCollision;
using namespace EDUtilities;
using namespace std;
using namespace EDGOCA;

namespace GameObjectMessages
{
	extern const MessageNameKey MSG_ATTRIBUTE_CHANGED;
}

using namespace GameObjectMessages;
namespace EDGOCA
{
	typedef int TransformKey;

	class DynAabbTreeLeafNode;

	struct TransformData
	{
		TransformKey transformKey;
		float4x4 localMatrix;
		TransformNode::TransformFlag transformFlag;
	};

	class GameObjectTemplate
	{
		friend class GameObject;
		friend class Scene;

		Aabb aabb;

		unsigned int id;
		unsigned int flags;

		set< BehaviorNameKey > behaviors;
		map< AttributeNameKey, IAttribute* > attributes;
		list< TransformData > transforms;
//		list< pair< TransformKey, float4x4> > transforms;
		list< pair<string, TransformKey> > childObjects;

	public:

		static GameObjectTemplate* LoadXML(const char* xmlFilePtr, const char* indentifier);

		const Aabb& GetAabb(void) const { return aabb; }

		unsigned int GetID(void){ return id; }

		unsigned int GetFlags(void){ return flags; }

		~GameObjectTemplate()
		{
			map< AttributeNameKey, IAttribute* >::iterator iter = attributes.begin();
			map< AttributeNameKey, IAttribute* >::iterator iterEnd = attributes.end();
			for(; iter != iterEnd; ++iter)
			{
				delete (*iter).second;
				(*iter).second = 0;
			}
		}
	};

	class GameObject : public TransformNode
	{
		friend class Scene;
		friend class IBehavior;

		Aabb aabb;
		Aabb localAabb;
		ContentHandle<GameObjectTemplate> gameObjectTemplateHandle;
		
		unsigned int gameObjectFlags;

		void DirtyNode(void);

		DynAabbTreeLeafNode* dynAabbTreeLeaf;

		DynAabbTreeLeafNode* visLeaf;

		map< BehaviorNameKey, IBehavior* > behaviors;
		map< AttributeNameKey, pair< IAttribute*, unsigned int > > attributes;
		map< MessageNameKey, set< IBehavior*, bool(*)(const IBehavior*, const IBehavior*) > > messageToBehaviorMap;

		map< GameObject*, TransformKey > childObjects;
		map< TransformKey, TransformNode* > transformSet;

		list< GameObject *> subObjects;

		void Update(void);

		IAttribute* GetAttribute( AttributeNameKey attributeNameKey );
		void DisableDraw(void);
		void EnableDraw(void);

		Scene* scene;

	public:
		
		enum GameObjectFlags{ AABB_INVALID = 1, STATIC = 2, ASLEEP = 4, UPDATEABLE = 8, DELETING = 16, UNIQUE = 32, MOVED = 64, NO_DRAW = 128 };

		GameObject(void);

		~GameObject();

		inline unsigned int GetGameObjectFlags(void) const { return gameObjectFlags; }

		const Aabb& GetAabb(void);
		const Aabb& GetLocalAabb(void){ return localAabb; }

		static void CollideObjects(GameObject* objectA, GameObject* objectB);

		void TranslateLocal(const float3& delta);
		void TranslateGlobal(const float3& delta);
		
		void RotateLocalX(float radians);
		void RotateLocalY(float radians);
		void RotateLocalZ(float radians);

		void RotateGlobalX(float radians);
		void RotateGlobalY(float radians);
		void RotateGlobalZ(float radians);

		void SetLocalTransform(const float4x4& transform);
		void SetLocalPosition(const float3& position);

		void AddBehavior( BehaviorNameKey behaviorNameKey );
		void RemoveBehavior( BehaviorNameKey behaviorNameKey );

		void OnMessage( IMessage* msg );
		void OnBroadcastMessage( IMessage* msg );
		void OnSendMessageUp( IMessage* msg );

		void BuildLocalAabb(void);

		GameObject* GetParentObject(void);
		GameObject* GetRootObject(void);

		NodeType GetNodeType(void){ return GAME_OBJECT; }
		
		void SetLocalTransform(TransformKey xformKey, const float4x4& xform);
		const float4x4& GetLocalTransform(TransformKey xformKey) const;
		const float4x4& GetLocalTransform(void)const;

		const float4x4& GetWorldTransform(TransformKey xformKey);
		const float4x4& GetWorldTransform(void);

		bool AddChildObject(GameObject* child, TransformKey xformKey);
		void RemoveChildObject(GameObject* child);

		GameObject *SpawnSubObject(const char* xmlFileNamePtr, const float4x4* xform = 0);


		template<typename type>
		class GOCAttribute
		{
			friend class GameObject;
		private:
			GameObject* gameObject;
			AttributeNameKey attribNameKey;

			GOCAttribute(GameObject* object, AttributeNameKey nameKey) : gameObject(object), attribNameKey(nameKey) {}

		public:

			operator type() const
			{
				return ((Attribute<type>*)(gameObject->GetAttribute(attribNameKey)))->value;
			}

			GOCAttribute & operator=(const type& rhs)
			{
				((Attribute<type>*)(gameObject->GetAttribute(attribNameKey)))->value = rhs;
				gameObject->OnMessage( &MessageT<AttributeNameKey>( MSG_ATTRIBUTE_CHANGED, attribNameKey ) );

				return *this;
			}
		};

		template<typename type>
		GOCAttribute<type> GetGOCAttribute(AttributeNameKey attributeNameKey)
		{
			return GOCAttribute<type>( this, attributeNameKey );
		}
	};
}