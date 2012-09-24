#include "LookAtBehavior.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
#include <assert.h>

namespace
{
	DeclareMessage( MSG_SET_TARGET );
	DeclareMessage( MSG_UPDATE );
}

namespace Behaviors
{
	void LookAtBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_SET_TARGET, OnSetTarget ) );
		AddMessageHandler( MessageHandler( MSG_UPDATE, OnUpdate ) );
	}

	void LookAtBehavior::OnSetTarget(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		assert(reinterpret_cast<LookAtBehavior *>(pInvokingBehavior));
		assert(reinterpret_cast<MessageT<GameObject *> *>(msg));

		LookAtBehavior *lookAtPtr = (LookAtBehavior *)pInvokingBehavior;
		MessageT<GameObject *> *message = (MessageT<GameObject *>*)msg;

		lookAtPtr->targetObjPtr = message->value;
	}

	void LookAtBehavior::OnUpdate(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		assert(reinterpret_cast<LookAtBehavior *>(pInvokingBehavior));

		LookAtBehavior *lookAtPtr = (LookAtBehavior *)pInvokingBehavior;
		
		if( lookAtPtr->targetObjPtr == 0 )
			return;

		float4x4 object = lookAtPtr->gameObject->GetLocalTransform();
		float3 objectPos = object.WAxis;
		float3 objectTargetPos = lookAtPtr->targetObjPtr->GetLocalTransform().WAxis;

		// TODO - comment this out and implement your own version
		//OnUpdateSolution(pInvokingBehavior, msg);
		float3 worldUp = float3( 0, 1, 0 );
		float3 zVec = objectTargetPos-objectPos;
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

		lookAtPtr->gameObject->SetLocalTransform( object );
		
	}
}
