#pragma once

#include "../EDGOCA/BehaviorT.h"

using namespace EDGOCA;

namespace Behaviors
{
	class WheelBehavior : public BehaviorT<WheelBehavior>
	{
	public:
		WheelBehavior(void)
		{
			baseSpeed = 15.0f;
		}

		~WheelBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnMoveForward( IBehavior* invokingBehavior, IMessage* message );
		static void OnMoveBackward( IBehavior* invokingBehavior, IMessage* message );

	private:
		float baseSpeed;

		float ComputeSpeed();
	};
}
