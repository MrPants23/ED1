#pragma once

#include "../EDGOCA/BehaviorT.h"

using namespace EDGOCA;

namespace Behaviors
{
	class Controller : public BehaviorT<Controller>
	{
	public:	
		Controller(void){}
	
		static void PopulateMessageMap(void);

		static void OnMoveForward( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveBackward( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveLeft( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveRight( IBehavior* invokingBehavior, IMessage* message );
		static void OnTurnLeft( IBehavior* invokingBehavior, IMessage* message );
		static void OnTurnRight( IBehavior* invokingBehavior, IMessage* message );
	};
}