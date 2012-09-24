#pragma once

#include "../EDGOCA/BehaviorT.h"

using namespace EDGOCA;

namespace Behaviors
{
	class SpinBehavior : public BehaviorT<SpinBehavior>
	{
	public:
		SpinBehavior(void)
		{
		    speed = 1.0f;
		}

		~SpinBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnUpdate( IBehavior* invokingBehavior, IMessage* message );
		static void OnSetMatrix( IBehavior* invokingBehavior, IMessage* message );
	private:
		float speed;
	};
}
