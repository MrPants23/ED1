#pragma once

#include "../EDGOCA/BehaviorT.h"

using namespace EDGOCA;

namespace Behaviors
{
	class TurretBehavior : public BehaviorT<TurretBehavior>
	{
	public:
		TurretBehavior(void)
		{
			baseSpeed = 15.0f;
		}

		~TurretBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnRotateRight( IBehavior* invokingBehavior, IMessage* message );
		static void OnRotateLeft( IBehavior* invokingBehavior, IMessage* message );
	    static void OnRightStick(IBehavior* pInvokingBehavior, IMessage* msg);
	    static void OnFire(IBehavior* pInvokingBehavior, IMessage* msg);

	private:
		float baseSpeed;

		float ComputeRotateSpeed();
	};
}
