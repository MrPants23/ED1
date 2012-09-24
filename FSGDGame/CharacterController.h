#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDUtilities/float3.h"

using namespace EDGOCA;
using namespace EDUtilities;

namespace Behaviors
{
	class CharacterController : public BehaviorT<CharacterController>
	{
		unsigned int m_MovementStateFlags;
		float3 m_PreviousPosition;
	public:

		enum MovementStateFlags{ IS_GROUNDED = 1, CAN_FLY = 2 };

		CharacterController(void) : m_MovementStateFlags(IS_GROUNDED) 
		{
			baseSpeed = 7.5f;
			baseRotSpeed = 4.0f;
		}

		void OnAdd(void);

		static void PopulateMessageMap(void);

		static void OnMoveForward( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveBackward( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveLeft( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveRight( IBehavior* invokingBehavior, IMessage* message );
		static void OnTurnLeft( IBehavior* invokingBehavior, IMessage* message );
		static void OnTurnRight( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveUp( IBehavior* invokingBehavior, IMessage* message );
		static void OnUpdate( IBehavior* invokingBehavior, IMessage* message );
		static void OnCollide( IBehavior* invokingBehavior, IMessage* message );
	private:
		float baseSpeed;
		float baseRotSpeed;

		float ComputeSpeed();
		float ComputeRotSpeed();
	};
}