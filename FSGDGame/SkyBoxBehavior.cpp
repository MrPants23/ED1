#include "SkyBoxBehavior.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "RenderController.h"
using namespace EDGOCA;
using namespace EDRendererD3D;

namespace
{
	DeclareMessage( MSG_BUILD_LOCAL_AABB );
}

namespace Behaviors
{
	void SkyBoxBehavior::PopulateMessageMap()
	{
		AddMessageHandler( MessageHandler( MSG_BUILD_LOCAL_AABB, OnBuildLocalAabb ) );
	}

	void SkyBoxBehavior::OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg )
	{
		SkyBoxBehavior *skyBoxBehPtr = (SkyBoxBehavior*)pInvokingBehavior;
		MessageT<Aabb*> *message = (MessageT<Aabb*>*)msg;

		// HACK : HACKZILLA!!! Just making the aabb for the skybox large so that it is always rendered, 
		// even though the skybox is actually a unit cube.  This should be based off the size of the 
		// world instead of hard coded.
		Aabb localAabb(float3(-100, -100, -100), float3(100, 100, 100));

		//const Aabb& localAabb = skyBoxBehPtr->m_RenderShape->GetRenderMesh()->GetAabb();
		for(unsigned int i = 0; i < 3; ++i)
		{
			if( message->value->min.v[i] > localAabb.min.v[i] )
				message->value->min.v[i] = localAabb.min.v[i];
			if( message->value->max.v[i] < localAabb.max.v[i] )
				message->value->max.v[i] = localAabb.max.v[i];
		}
	}
}