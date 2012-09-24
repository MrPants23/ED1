#include "GroundClamp.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"

using namespace EDGOCA;

namespace
{
	DeclareMessage( MSG_COLLIDE );
	DeclareMessage( MSG_INTERSECT_SEGMENT );
	DeclareMessage( MSG_UPDATE );
}

namespace
{
	DeclareAttribute(bool, GROUNDED_ATTRIB);
}

namespace Behaviors
{
	const AttributeNameKey* GroundClamp::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			GROUNDED_ATTRIB,
			0
		};

		return attribs;
	}

	void GroundClamp::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_COLLIDE, OnCollide ) );
		AddMessageHandler( MessageHandler( MSG_UPDATE, OnUpdate ) );
	}

	void GroundClamp::OnCollide( IBehavior* invokingBehavior, IMessage* message )
	{
		GroundClamp* pGroundClamp = (GroundClamp*)invokingBehavior;

		MessageT<GameObject*>* msg = (MessageT<GameObject*>*)message;
		
		MessageT< pair<bool, Segment> > segMsg( MSG_INTERSECT_SEGMENT );
		segMsg.value.first = false;
		segMsg.value.second.start = (pGroundClamp->gameObject->GetAabb().min + pGroundClamp->gameObject->GetAabb().max) * 0.5f;
		segMsg.value.second.end = segMsg.value.second.start;
		segMsg.value.second.end.y = pGroundClamp->gameObject->GetAabb().min.y;
		float3 endPt = segMsg.value.second.end;

		msg->value->OnMessage( &segMsg );

		Attribute<bool>* groundedAttrib = (Attribute<bool>*)pGroundClamp->GetAttribute(GROUNDED_ATTRIB);
		if( segMsg.value.first == true )
		{
			groundedAttrib->value = true;
			pGroundClamp->gameObject->TranslateGlobal( segMsg.value.second.end - endPt );
		}
	}

	void GroundClamp::OnUpdate( IBehavior* invokingBehavior, IMessage* message )
	{
		GroundClamp* pGroundClamp = (GroundClamp*)invokingBehavior;

		Attribute<bool>* groundedAttrib = (Attribute<bool>*)pGroundClamp->GetAttribute(GROUNDED_ATTRIB);
		groundedAttrib->value = false;
	}
}