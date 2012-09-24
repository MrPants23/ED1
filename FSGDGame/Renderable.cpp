#include "Renderable.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "RenderController.h"
using namespace EDGOCA;
using namespace EDRendererD3D;

#include "AttributeReaders.h"

namespace
{
	DeclareMessage( MSG_SET_RENDERSHAPE );
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
	DeclareMessage( MSG_GET_RENDERABLE_COUNT );
}

namespace
{
	DeclareAttribute( string, RENDERABLE_PATH_ATTRIB  );
}

namespace Behaviors
{
	void Renderable::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( MSG_SET_RENDERSHAPE, OnSetRenderShape ) );
		AddMessageHandler( MessageHandler( MSG_FRUSTUM_TEST, OnFrustumTest ) );
		AddMessageHandler( MessageHandler( MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb ) );
		AddMessageHandler( MessageHandler( MSG_GET_RENDERABLE_COUNT, OnGetRenderableCount ) );
		AddMessageHandler( MSG_ATTRIBUTE_CHANGED, OnAttributeChanged );		
	}

	const AttributeNameKey* Renderable::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			RENDERABLE_PATH_ATTRIB,
			0
		};

		return attribs;		
	}

	void Renderable::OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		Renderable* pThis = (Renderable*)pInvokingBehavior;
		MessageT<AttributeNameKey>* message = (MessageT<AttributeNameKey>*)msg;

		if( message->value == RENDERABLE_PATH_ATTRIB )
		{
			Attribute<string>* renderablePathAttrib = (Attribute<string>*)pThis->GetAttribute( message->value );
			MessageT< pair<BehaviorNameKey, string> > setRenderShapeMsg( MSG_SET_RENDERSHAPE, pair<BehaviorNameKey, string>( pThis->GetBehaviorNameKey(), renderablePathAttrib->value ) );
			pThis->gameObject->OnMessage( &setRenderShapeMsg );
			
			return;
		}
	}
}