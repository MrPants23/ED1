#include "Physics.h"

#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDGOCA/Attribute.h"
#include "CollisionContact.h"

using namespace EDGOCA;

#define TIME_STEP 0.01666666f
#define TIME_STEP_SQ (TIME_STEP*TIME_STEP)

namespace
{
	DeclareMessage( MSG_UPDATE );
	DeclareMessage( MSG_APPLY_FORCE );
	DeclareMessage( MSG_APPLY_REACTION );
}

namespace
{
	DeclareAttribute(bool, GROUNDED_ATTRIB);
}

namespace Behaviors
{
	void Physics::OnAdd(void)
	{
		Attribute<bool>* groundedAttrib = (Attribute<bool>*)GetAttribute(GROUNDED_ATTRIB);
		groundedAttrib->value = false;
	}

	const AttributeNameKey* Physics::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			GROUNDED_ATTRIB,
			0
		};

		return attribs;
	}

	void Physics::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_UPDATE, OnUpdate ) );
		AddMessageHandler( MessageHandler( MSG_APPLY_FORCE, OnApplyForce ) );
		AddMessageHandler( MessageHandler( MSG_APPLY_REACTION, OnApplyReaction ) );
	}

	void Physics::OnUpdate(IBehavior* invokingBehavior, IMessage* message)
	{
		Physics* pPhysics = (Physics*)invokingBehavior;

		const Aabb& objLocalAabb = pPhysics->gameObject->GetLocalAabb();
		for(unsigned int i = 0; i < 3; ++i)
		{
			if( pPhysics->localAabb.min.v[i] != objLocalAabb.min.v[i] ||
				pPhysics->localAabb.max.v[i] != objLocalAabb.max.v[i] )
			{
				pPhysics->Initialize();
				return;
			}
		}

		if( pPhysics->m_PhysicsFlags & GRAVITY )
		{
			Attribute<bool>* groundedAttrib = (Attribute<bool>*)pPhysics->GetAttribute(GROUNDED_ATTRIB);

			if( groundedAttrib->value == false )
				pPhysics->ApplyGravity();
		}

		float3 com(0.0f, 0.0f, 0.0f);
		for(unsigned int i = 0; i < pPhysics->m_X.size(); ++i)
			com += pPhysics->m_X[i];
		
		com *= (1.0f / pPhysics->m_X.size());

		pPhysics->ApplyVerlet();
		pPhysics->ApplyConstraints();

		float3 newcom(0.0f, 0.0f, 0.0f);
		for(unsigned int i = 0; i < pPhysics->m_X.size(); ++i)
			newcom += pPhysics->m_X[i];
		
		newcom *= (1.0f / pPhysics->m_X.size());

		float3 delta = newcom - com;

		if( delta.magnitude() > 0.25f )
		{
			delta.normalize();
			delta *= 0.25f;
		}

		pPhysics->gameObject->TranslateGlobal( delta );
	}

	void Physics::OnApplyForce(IBehavior* invokingBehavior, IMessage* message)
	{
		Physics* pPhysics = (Physics*)invokingBehavior;
		MessageT<float3>* msg = (MessageT<float3>*)message;

		if( (pPhysics->m_PhysicsFlags & DOF_3) )
		{
			pPhysics->m_a[0] += msg->value;
		}
	}

	void Physics::OnApplyReaction(IBehavior* invokingBehavior, IMessage* message)
	{
		Physics* pPhysics = (Physics*)invokingBehavior;
		MessageT<CollisionContact>* msg = (MessageT<CollisionContact>*)message;
	
		float3 velocity = pPhysics->m_X[0] - pPhysics->m_OldX[0];
		
		velocity += msg->value.m_ContactNormal * msg->value.m_PenetrationDepth;

		float ndotv = DotProduct( msg->value.m_ContactNormal, velocity );
		float3 normalVelocity = msg->value.m_ContactNormal * ndotv;
		float3 tangentialVelocity = velocity - normalVelocity;
		float3 oldTV = tangentialVelocity;

		float m_COF = 0.0f;
		float m_COR = 0.0f;

		float tvMagSq = DotProduct(tangentialVelocity, tangentialVelocity);
		if( tvMagSq > 0.0f )
		{
			float tvMag = sqrt(tvMagSq);
			tangentialVelocity *= (1.0f / tvMag);
			float3 tvNorm = tangentialVelocity;
			tangentialVelocity *= max( 0.0f, tvMag - msg->value.m_PenetrationDepth*m_COF);				
		}
		else
			tangentialVelocity.makeZero();

		pPhysics->gameObject->TranslateGlobal( velocity );
		pPhysics->m_X[0] = pPhysics->m_OldX[0] + velocity;
		pPhysics->m_OldX[0] = pPhysics->m_X[0] - tangentialVelocity - normalVelocity * m_COR;

		//pPhysics->m_OldX[0] += (oldTV - tangentialVelocity);
		//pPhysics->m_OldX[0] = pPhysics->m_X[0] - tangentialVelocity - normalVelocity;
		//pPhysics->m_OldX[0] -= normalVelocity;
	}

	void Physics::ApplyVerlet(void)
	{
		for(unsigned int i = 0; i < m_X.size(); ++i)
		{
			float3 newPos = m_X[i] + ((m_X[i] - m_OldX[i])*0.99f) + m_a[i] * TIME_STEP_SQ;
			m_OldX[i] = m_X[i];
			m_X[i] = newPos;

			m_a[i].makeZero();
		}
	}

	void Physics::ApplyConstraints(void)
	{
	}

	void Physics::ApplyGravity(void)
	{
		for(unsigned int i = 0; i < m_a.size(); ++i)
		{
			m_a[i].y -= 9.8f;
		}
	}

	void Physics::Initialize(void)
	{
		if( (m_PhysicsFlags & DOF_3) )
		{
			if( m_X.size() < 1 )
			{
				m_X.resize(1);
				m_OldX.resize(1);
				m_a.resize(1);
			}

			localAabb = gameObject->GetLocalAabb();
			TranslateVector( m_X[0], (localAabb.min + localAabb.max)*0.5f, gameObject->GetWorldTransform() );
			m_OldX[0] = m_X[0];
			m_a[0].makeZero();
		}
		else if( (m_PhysicsFlags & DOF_6) )
		{
		}
	}
}