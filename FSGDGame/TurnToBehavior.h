#pragma once

#include "../EDGOCA/BehaviorT.h"
using namespace EDGOCA;

#define TURN_TO_ROTATION_EPSILON		0.025f
#define TURN_TO_ROTATION_SPEED			1.3f

namespace Behaviors
{
	class TurnToBehavior : public BehaviorT<TurnToBehavior>
	{
	public:
		TurnToBehavior(void) : m_pTargetObj(0) {}
		~TurnToBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnSetTarget(IBehavior* pInvokingBehavior, IMessage* msg);
		static void OnUpdate(IBehavior* pInvokingBehavior, IMessage* msg);

		static void OnUpdateSolution(IBehavior* pInvokingBehavior, IMessage* msg);

	private:

		GameObject *m_pTargetObj;
	};
}
