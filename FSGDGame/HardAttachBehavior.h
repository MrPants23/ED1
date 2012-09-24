#pragma once

#include "../EDGOCA/BehaviorT.h"
using namespace EDGOCA;

namespace Behaviors
{
	class HardAttachBehavior : public BehaviorT<HardAttachBehavior>
	{
	public:
		HardAttachBehavior(void) {}
		~HardAttachBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnAttach(IBehavior* pInvokingBehavior, IMessage* msg);
		static void OnSetOffset(IBehavior* pInvokingBehavior, IMessage* msg);
	};
}