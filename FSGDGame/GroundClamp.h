#pragma once

#include "../EDGOCA/BehaviorT.h"

using namespace EDGOCA;

namespace Behaviors
{
	class GroundClamp : public BehaviorT<GroundClamp>
	{
		bool isEnabled;
	public:
		GroundClamp(void) : isEnabled(true) {}
		
		const AttributeNameKey* GetRequiredAttributes(void);

		static void PopulateMessageMap(void);

		static void OnCollide( IBehavior* invokingBehavior, IMessage* message );
		static void OnUpdate( IBehavior* invokingBehavior, IMessage* message );
	};
}