#pragma once

#include "../EDGOCA/BehaviorT.h"

using namespace EDGOCA;

namespace Behaviors
{
	class ProjectileBehavior : public BehaviorT<ProjectileBehavior>
	{
	public:
		ProjectileBehavior(void)
		{
			baseSpeed = 7.5f;
		}

		~ProjectileBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnUpdate( IBehavior* invokingBehavior, IMessage* message );


	private:
		float baseSpeed;

		float ComputeSpeed();
	};
}
