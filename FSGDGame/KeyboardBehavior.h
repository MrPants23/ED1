#pragma once

#include "../EDGOCA/BehaviorT.h"
using namespace EDGOCA;

#include <map>
using std::map;

namespace Behaviors
{
	class KeyboardBehavior : public BehaviorT<KeyboardBehavior>
	{
	public:
		KeyboardBehavior(void);
		~KeyboardBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnInitialize(IBehavior* pInvokingBehavior, IMessage* msg);

	    static void OnKeyCallback(unsigned int eventId, void *eventData, void *listener);

	private:
		map<unsigned int, MessageNameKey > events;
	};
}
