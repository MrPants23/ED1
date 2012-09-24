#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDCollision/CollidableTypes.h"

using namespace EDCollision;
using namespace EDGOCA;

namespace Behaviors
{
	class Physics : public BehaviorT<Physics>
	{
		struct EdgeConstraint
		{
			unsigned int indices[2];
			float m_RestLength;
		};

	public:
		enum PhysicsFlags{ GRAVITY = 1, DOF_3 = 2, DOF_6 = 4 };

		Physics(void) : m_PhysicsFlags(GRAVITY|DOF_3) {}

		static void PopulateMessageMap(void);

		static void OnUpdate(IBehavior* invokingBehavior, IMessage* message);
		static void OnApplyForce(IBehavior* invokingBehavior, IMessage* message);
		static void OnApplyReaction(IBehavior* invokingBehavior, IMessage* message);
		
		const AttributeNameKey* GetRequiredAttributes(void);

		void OnAdd(void);

	private:
		
		unsigned int m_PhysicsFlags;
		Aabb localAabb;

		vector<float3> m_X;
		vector<float3> m_OldX;
		vector<float3> m_a;

		vector<EdgeConstraint> m_InternalConstraints;

		void ApplyVerlet(void);
		void ApplyConstraints(void);
		void ApplyGravity(void);
		void Initialize(void);
	};
}