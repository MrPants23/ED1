#include "DirectionalLightController.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;

#include "../EDRendererD3D/RenderModel.h"
#include "../EDRendererD3D/RenderShape.h"
using namespace EDRendererD3D;

namespace
{
	DeclareMessage( MSG_SET_DIRECTIONAL_LIGHT );
	DeclareMessage( MSG_SET_DIRECTIONAL_LIGHT_CONTEXT );
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
	DeclareMessage( MSG_GET_RENDERABLE_COUNT );
}

namespace Behaviors
{
	void DirectionalLightController::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( MSG_SET_DIRECTIONAL_LIGHT, OnSetLight ) );
		AddMessageHandler( MessageHandler( MSG_SET_DIRECTIONAL_LIGHT_CONTEXT, OnSetRenderContext ) );
		AddMessageHandler( MessageHandler( MSG_FRUSTUM_TEST, OnFrustumTest ) );
		AddMessageHandler( MessageHandler( MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb ) );
		AddMessageHandler( MSG_GET_RENDERABLE_COUNT, OnGetRenderableCount );
	}

	void DirectionalLightController::OnSetLight( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		DirectionalLightController* lightBehaviorPtr = (DirectionalLightController*)pInvokingBehavior;
		MessageT<DirectionalLight *>* message = (MessageT<DirectionalLight *>*)msg;

		lightBehaviorPtr->lightPtr = (DirectionalLight *)message->value;

		lightBehaviorPtr->gameObject->BuildLocalAabb();
	}

	void DirectionalLightController::OnSetRenderContext( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		DirectionalLightController* lightBehaviorPtr = (DirectionalLightController*)pInvokingBehavior;
		MessageT<RenderContext *>* message = (MessageT<RenderContext *>*)msg;

		lightBehaviorPtr->contextPtr = (RenderContext *)message->value;
	}

	void DirectionalLightController::OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		DirectionalLightController* lightBehaviorPtr = (DirectionalLightController*)pInvokingBehavior;
		MessageT<FrustumHull*>* message = (MessageT<FrustumHull*>*)msg;

		if( !lightBehaviorPtr->lightPtr || !lightBehaviorPtr->contextPtr)
			return;

		Aabb tempAabb = lightBehaviorPtr->gameObject->GetAabb();
		if( message->value->Collides( &tempAabb ) )
		{
			lightBehaviorPtr->lightPtr->ApplyLight(lightBehaviorPtr->contextPtr->GetEffect());
		}
	}

	void DirectionalLightController::OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		DirectionalLightController* lightBehaviorPtr = (DirectionalLightController*)pInvokingBehavior;
		MessageT<Aabb*>* message = (MessageT<Aabb*>*)msg;

		// TODO : Define an AABB for the area that the directional light should effect
		Aabb localAabb(float3(-100, -100, -100), float3(100, 100, 100));
		//const Aabb& localAabb = 
			//lightBehaviorPtr->lightPtr->GetRenderModel()->GetShapes()[0]->GetRenderMesh()->GetAabb();
		for(unsigned int i = 0; i < 3; ++i)
		{
			if( message->value->min.v[i] > localAabb.min.v[i] )
				message->value->min.v[i] = localAabb.min.v[i];
			if( message->value->max.v[i] < localAabb.max.v[i] )
				message->value->max.v[i] = localAabb.max.v[i];
		}
	}

	void DirectionalLightController::OnGetRenderableCount( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		MessageT<unsigned int>* message = (MessageT<unsigned int>*)msg;
		message->value += 1;
	}
}
