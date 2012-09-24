#include "CharacterController.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;

#include "WorldData.h"

#define TIME_STEP 0.01666666f
#define TIME_STEP_SQ (TIME_STEP*TIME_STEP)

namespace
{
	DeclareMessage( MSG_INTERSECT_SEGMENT );
	DeclareMessage( MSG_COLLIDE );
	DeclareMessage( MSG_UPDATE );
	DeclareMessage( MSG_MOVE_FORWARD );
	DeclareMessage( MSG_MOVE_UP );
	DeclareMessage( MSG_MOVE_BACKWARD );
	DeclareMessage( MSG_TURN_LEFT );
	DeclareMessage( MSG_TURN_RIGHT );
}

namespace Behaviors
{
	void CharacterController::OnAdd(void)
	{
		m_PreviousPosition = gameObject->GetWorldTransform().WAxis;
	}

	void CharacterController::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_COLLIDE, OnCollide ) );
		AddMessageHandler( MessageHandler( MSG_UPDATE, OnUpdate ) );
		AddMessageHandler( MessageHandler( MSG_MOVE_FORWARD, OnMoveForward ) );
		AddMessageHandler( MessageHandler( MSG_MOVE_BACKWARD, OnMoveBackward ) );
		AddMessageHandler( MessageHandler( MSG_MOVE_UP, OnMoveUp ) );
		AddMessageHandler( MessageHandler( MSG_TURN_LEFT, OnTurnLeft ) );
		AddMessageHandler( MessageHandler( MSG_TURN_RIGHT, OnTurnRight ) );
	}

	void CharacterController::OnMoveForward( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* controller = (CharacterController*)invokingBehavior;

		if( controller->m_MovementStateFlags & IS_GROUNDED )
			controller->gameObject->TranslateLocal( float3( 0.0f, 0.0f, 1.0f )  * controller->ComputeSpeed());
	}

	void CharacterController::OnMoveBackward( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* controller = (CharacterController*)invokingBehavior;

		if( controller->m_MovementStateFlags & IS_GROUNDED )
			controller->gameObject->TranslateLocal( float3( 0.0f, 0.0f, -1.0f )  * controller->ComputeSpeed());
	}

	void CharacterController::OnMoveLeft( IBehavior* invokingBehavior, IMessage* message )
	{
	}

	void CharacterController::OnMoveRight( IBehavior* invokingBehavior, IMessage* message )
	{
	}

	void CharacterController::OnTurnLeft( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* pController = (CharacterController*)invokingBehavior;

		pController->gameObject->RotateLocalY(-pController->ComputeRotSpeed());
	}

	void CharacterController::OnTurnRight( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* pController = (CharacterController*)invokingBehavior;

		pController->gameObject->RotateLocalY(pController->ComputeRotSpeed());
	}

	void CharacterController::OnMoveUp( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* controller = (CharacterController*)invokingBehavior;

		if( controller->m_MovementStateFlags & IS_GROUNDED )
		{
			controller->gameObject->TranslateLocal( float3( 0.0f, 1.0f, 0.0f )  * controller->ComputeSpeed());
			controller->m_MovementStateFlags &= ~IS_GROUNDED;
		}
	}

	void CharacterController::OnUpdate( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* controller = (CharacterController*)invokingBehavior;

		if( controller->m_MovementStateFlags & IS_GROUNDED )
			controller->m_MovementStateFlags &= ~IS_GROUNDED;
		else
		{
			float3 gravity(0.0f, -9.8f, 0.0f);
			float3 pos = controller->gameObject->GetWorldTransform().WAxis;

			float3 newPos = pos + ((pos - controller->m_PreviousPosition)*0.98f) + gravity * TIME_STEP_SQ;
			float3 delta = newPos - pos;
			controller->gameObject->TranslateGlobal(delta);
			controller->m_PreviousPosition = pos;
		}
	}

	void CharacterController::OnCollide( IBehavior* invokingBehavior, IMessage* message )
	{
		CharacterController* controller = (CharacterController*)invokingBehavior;
		MessageT<GameObject*>* msg = (MessageT<GameObject*>*)message;
		
		MessageT< pair<bool, Segment> > segMsg( MSG_INTERSECT_SEGMENT );
		segMsg.value.first = false;
		segMsg.value.second.start = (controller->gameObject->GetAabb().min + controller->gameObject->GetAabb().max) * 0.5f;
		segMsg.value.second.end = segMsg.value.second.start;
		segMsg.value.second.end.y = controller->gameObject->GetAabb().min.y;
		float3 endPt = segMsg.value.second.end;
		segMsg.value.second.end.y -= 0.125f;

		msg->value->OnMessage( &segMsg );

		if( segMsg.value.first == true )
		{
			controller->m_MovementStateFlags |= IS_GROUNDED;
			controller->gameObject->TranslateGlobal( (segMsg.value.second.end - endPt) );
			controller->m_PreviousPosition = controller->gameObject->GetWorldTransform().WAxis;
		}
	}

	float CharacterController::ComputeSpeed()
	{
		return baseSpeed * WorldData::GetDeltaTime();
	}

	float CharacterController::ComputeRotSpeed()
	{
		return baseRotSpeed * WorldData::GetDeltaTime();
	}
}