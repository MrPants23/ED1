#pragma once

#include "../EDGOCA/BehaviorT.h"
using namespace EDGOCA;

#include <map>
using std::map;

namespace Behaviors
{
	class JoystickBehavior : public BehaviorT<JoystickBehavior>
	{
	public:
		JoystickBehavior(void);
		~JoystickBehavior(void) {}

		static void PopulateMessageMap(void);

		static void OnInitialize(IBehavior* pInvokingBehavior, IMessage* msg);
	    static void OnLeftStick(IBehavior* pInvokingBehavior, IMessage* msg);
	    static void OnRightStick(IBehavior* pInvokingBehavior, IMessage* msg);

	    static void OnInputCallback(unsigned int eventId, void *eventData, void *listener);

	private:
		map<unsigned int, MessageNameKey > events;

		float translateSpeed;
		float rotateSpeed;

		float ComputeTranslateSpeed();
		float ComputeRotateSpeed();
	};
}
