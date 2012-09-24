#include "ProjectileBehavior.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;

#include "WorldData.h"

namespace
{
	DeclareMessage( MSG_UPDATE );
	DeclareMessage( MSG_MOVE_BACKWARD );
	DeclareMessage( MSG_MOVE_LEFT );
	DeclareMessage( MSG_MOVE_RIGHT );
	DeclareMessage( MSG_MOVE_UP );
	DeclareMessage( MSG_MOVE_DOWN );
}

namespace Behaviors
{
	void ProjectileBehavior::PopulateMessageMap(void)
	{
		AddMessageHandler( MessageHandler( MSG_UPDATE, OnUpdate ) );
	}

	void ProjectileBehavior::OnUpdate( IBehavior* invokingBehavior, IMessage* message )
	{
		ProjectileBehavior *behaviorPtr = (ProjectileBehavior *)invokingBehavior;

		behaviorPtr->gameObject->TranslateLocal(float3(0, 0, 1) * behaviorPtr->ComputeSpeed());
	}

	float ProjectileBehavior::ComputeSpeed()
	{
		return baseSpeed * WorldData::GetDeltaTime();
	}
}
