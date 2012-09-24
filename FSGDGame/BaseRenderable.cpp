#include "BaseRenderable.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "RenderController.h"
using namespace EDGOCA;
using namespace EDRendererD3D;

using namespace Behaviors;

namespace
{
	DeclareMessage( MSG_SET_BASE_RENDERSHAPE_LIST );
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
	DeclareMessage( MSG_GET_RENDERABLE_COUNT );
}

template<>
void Attribute<RenderShapePathList>::ReadXML(TiXmlElement* valElement)
{
	TiXmlElement* renderShapeEle = valElement->FirstChildElement("RenderShape");
	
	for(; renderShapeEle != 0; renderShapeEle = renderShapeEle->NextSiblingElement("RenderShape") )
	{
		pair< RenderShapeLODPaths, TransformKey > renderShapePaths;
		renderShapePaths.second = -1;

		TiXmlElement* xformNameEle = renderShapeEle->FirstChildElement("TransformName");
		if( xformNameEle != 0 && xformNameEle->GetText() != 0 )
			renderShapePaths.second = static_cast<TransformKey>(hash<string>()( xformNameEle->GetText() ));
		
		TiXmlElement* lodPathEle = renderShapeEle->FirstChildElement("Path");
		for(; lodPathEle != 0; lodPathEle = lodPathEle->NextSiblingElement("Path") )
		{
			if( lodPathEle->GetText() != 0 )
				renderShapePaths.first.push_back( lodPathEle->GetText() );
		}

		if( renderShapePaths.first.size() != 0 )
			this->value.push_back( renderShapePaths );
	}
}

namespace
{
	DeclareAttribute( RenderShapePathList, BASE_RENDERABLE_SHAPE_PATH_LIST  );
}

namespace Behaviors
{
	void BaseRenderable::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( MSG_SET_BASE_RENDERSHAPE_LIST, OnSetRenderShapeList ) );
		AddMessageHandler( MessageHandler( MSG_FRUSTUM_TEST, OnFrustumTest ) );
		AddMessageHandler( MessageHandler( MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb ) );
		AddMessageHandler( MessageHandler( MSG_GET_RENDERABLE_COUNT, OnGetRenderableCount ) );
		AddMessageHandler( MSG_ATTRIBUTE_CHANGED, OnAttributeChanged );		
	}

	const AttributeNameKey* BaseRenderable::GetRequiredAttributes(void)
	{
		static AttributeNameKey attribs[] =
		{
			BASE_RENDERABLE_SHAPE_PATH_LIST,
			0
		};

		return attribs;		
	}

	void BaseRenderable::OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		BaseRenderable* pThis = (BaseRenderable*)pInvokingBehavior;
		MessageT<AttributeNameKey>* message = (MessageT<AttributeNameKey>*)msg;
		
		if( message->value == BASE_RENDERABLE_SHAPE_PATH_LIST )
		{
			Attribute<RenderShapePathList>* renderShapePathList = (Attribute<RenderShapePathList>*)pThis->GetAttribute( message->value );

			MessageT< RenderShapePathList* > setShapeListMessage( MSG_SET_BASE_RENDERSHAPE_LIST );
			setShapeListMessage.value = &renderShapePathList->value;
			pThis->gameObject->OnMessage( &setShapeListMessage );
		}
	}
}