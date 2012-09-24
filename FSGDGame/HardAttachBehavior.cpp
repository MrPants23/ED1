#include "HardAttachBehavior.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"

namespace
{
	DeclareMessage( MSG_ATTACH );
	DeclareMessage( MSG_SET_OFFSET );
}

namespace Behaviors
{
	void HardAttachBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_ATTACH, OnAttach ) );
		AddMessageHandler( MessageHandler( MSG_SET_OFFSET, OnSetOffset ) );

	}

	void HardAttachBehavior::OnAttach(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		HardAttachBehavior *attachBehavior = (HardAttachBehavior*)pInvokingBehavior;
		MessageT<GameObject *> *message = (MessageT<GameObject *>*)msg;
		//message->value->AddChild(attachBehavior->gameObject);
		message->value->AddChildObject(attachBehavior->gameObject, -1);
	}

	void HardAttachBehavior::OnSetOffset(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		HardAttachBehavior *attachBehavior = (HardAttachBehavior*)pInvokingBehavior;
		MessageT<float3 *> *message = (MessageT<float3 *>*)msg;
		attachBehavior->gameObject->SetLocalPosition(*message->value);
	}
}