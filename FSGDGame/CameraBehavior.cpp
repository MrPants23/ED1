#include "CameraBehavior.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDGOCA/GameObject.h"
#include "../EDRendererD3D/ViewPortManager.h"
using EDRendererD3D::ViewPortManager;

namespace
{
	DeclareMessage(MSG_SET_PORT_INDEX);
	DeclareMessage(MSG_BUILD_PROJECTION);
	DeclareMessage(MSG_UPDATE);
}

namespace Behaviors
{
	void CameraBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_SET_PORT_INDEX, OnSetViewPortIndex ) );
		AddMessageHandler( MessageHandler( MSG_UPDATE, OnUpdate ) );
		AddMessageHandler( MessageHandler( MSG_BUILD_PROJECTION, OnBuildProjection ) );		
	}

	void CameraBehavior::OnSetViewPortIndex(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		CameraBehavior *cameraBehavior = (CameraBehavior*)pInvokingBehavior;
		MessageT<UINT> *message = (MessageT<UINT>*)msg;

		cameraBehavior->viewPortIndex = message->value;
	}

	void CameraBehavior::OnBuildProjection(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		CameraBehavior *cameraBehavior = (CameraBehavior*)pInvokingBehavior;
		MessageT<float *> *message = (MessageT<float *>*)msg;
		float width = message->value[0];
		float height = message->value[1];
		cameraBehavior->nearClip = message->value[2];
		cameraBehavior->farClip = message->value[3];
		cameraBehavior->projection.makePerspectiveLH(D3DX_PI/4.0f, width/height, 
			cameraBehavior->nearClip, cameraBehavior->farClip);
	}

	void CameraBehavior::OnUpdate(IBehavior* pInvokingBehavior, IMessage* msg)
	{
		CameraBehavior *cameraBehavior = (CameraBehavior*)pInvokingBehavior;
		float4x4 world = cameraBehavior->gameObject->GetWorldTransform();
		float4x4 inverseWorld = world;
		inverseWorld.transpose3x3();
		float3 dotProducts = float3(-(world.WAxis.dotProduct(world.XAxis)),
			-(world.WAxis.dotProduct(world.YAxis)),
			-(world.WAxis.dotProduct(world.ZAxis)));
		inverseWorld.WAxis = dotProducts;

		// TODO - comment this out and implement your own version
		//OnUpdateSolution(pInvokingBehavior, msg);

		// HINT - you are calculating WORLD and VIEW, when done you need to update the viewport
		//EDRendererD3D::ViewPortManager::GetReference().UpdateViewPort(
		//	cameraBehavior->viewPortIndex, THE_WORLD_MATRIX, THE_VIEW_MATRIX, cameraBehavior->projection, 
		//	cameraBehavior->nearClip, cameraBehavior->farClip);
		EDRendererD3D::ViewPortManager::GetReference().UpdateViewPort( cameraBehavior->viewPortIndex, world, inverseWorld, cameraBehavior->projection, cameraBehavior->nearClip, cameraBehavior->farClip ); 
	}
}
