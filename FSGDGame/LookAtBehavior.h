#pragma once

#include "../EDGOCA/BehaviorT.h"
using namespace EDGOCA;

namespace Behaviors
{
	class LookAtBehavior : public BehaviorT<LookAtBehavior>
	{
	public:
		LookAtBehavior(void) : targetObjPtr(0) {}
		~LookAtBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnSetTarget(IBehavior* pInvokingBehavior, IMessage* msg);
		static void OnUpdate(IBehavior* pInvokingBehavior, IMessage* msg);

		static void OnUpdateSolution(IBehavior* pInvokingBehavior, IMessage* msg);

	private:

		GameObject *targetObjPtr;
	};
}
