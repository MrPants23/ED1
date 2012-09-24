#include "TurnToBehavior.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
#include "../FSGDGame/WorldData.h"
#include <assert.h>

namespace
{
	DeclareMessage( MSG_SET_TARGET );
	DeclareMessage( MSG_UPDATE );
}

namespace Behaviors
{
	void TurnToBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_SET_TARGET, OnSetTarget ) );
		AddMessageHandler( MessageHandler( MSG_UPDATE, OnUpdate ) );
	}

	void TurnToBehavior::OnSetTarget(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		assert(reinterpret_cast<TurnToBehavior *>(pInvokingBehavior));
		assert(reinterpret_cast<MessageT<GameObject *> *>(msg));

		TurnToBehavior *turnToPtr = (TurnToBehavior *)pInvokingBehavior;
		MessageT<GameObject *> *message = (MessageT<GameObject *>*)msg;

		turnToPtr->m_pTargetObj = message->value;
	}

	void TurnToBehavior::OnUpdate(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		assert(reinterpret_cast<TurnToBehavior *>(pInvokingBehavior));

		TurnToBehavior *turnToPtr = (TurnToBehavior *)pInvokingBehavior;
		
		if( turnToPtr->m_pTargetObj == 0 )
			return;

		float4x4 object = turnToPtr->gameObject->GetLocalTransform();
		float3 objectPos = object.WAxis;
		float3 objectTargetPos = turnToPtr->m_pTargetObj->GetLocalTransform().WAxis;

		// TODO - comment this out and implement your own version
		//OnUpdateSolution(pInvokingBehavior, msg);
		float3 toTarget = objectTargetPos- objectPos;
		float xDot = toTarget.dotProduct(object.XAxis );

		/*if( (fabs(xDot))<0.00001f )
			xDot = 0.0f;*/
		float temp = toTarget.x;
		toTarget.x = 0.0f;
		/*object = */object.rotateLocalY(xDot*WorldData::GetDeltaTime());
		toTarget.x = temp;

		float yDot = toTarget.dotProduct( object.YAxis );

		/*if( (fabs(yDot))<0.00001f )
			yDot = 0.0f;*/
		temp = toTarget.y;
		toTarget.y = 0.0f;
		/*object = */object.rotateLocalX(-yDot*WorldData::GetDeltaTime());
		toTarget.y = temp;

		float3 worldUp = float3( 0, 1, 0 );
		float3 zVec = object.ZAxis;
		zVec = zVec.normalize();
		float3 xVec; 
		CrossProduct( xVec, worldUp, zVec );
		xVec = xVec.normalize();
		float3 yVec;
		CrossProduct( yVec, zVec, xVec );
		yVec = yVec.normalize();

		object.XAxis = xVec;
		object.YAxis = yVec;
		object.ZAxis = zVec;

		turnToPtr->gameObject->SetLocalTransform( object );
	}
}
