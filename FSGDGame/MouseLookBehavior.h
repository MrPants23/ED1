#pragma once

#include "../EDGOCA/BehaviorT.h"
using namespace EDGOCA;

// this could be made part of the class to have different mouse speeds per object
#define MOUSE_LOOK_SPEED_MODIFIER			0.5f

namespace Behaviors
{
	class MouseLookBehavior : public BehaviorT<MouseLookBehavior>
	{
	public:
		MouseLookBehavior(void)
		{
			rotateEnabled = false;
		}
		~MouseLookBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnBeginRotate(IBehavior* pInvokingBehavior, IMessage* msg);
		static void OnEndRotate(IBehavior* pInvokingBehavior, IMessage* msg);
		static void OnRotate(IBehavior* pInvokingBehavior, IMessage* msg);
		static void OnInitialize(IBehavior* pInvokingBehavior, IMessage* msg);
	    static void OnMouseEventCallback(unsigned int eventId, void *eventData, void *listener);

		static void OnRotateSolution(IBehavior* pInvokingBehavior, IMessage* msg);

	private:
		long cursorPosX;
		long cursorPosY;
		bool rotateEnabled;
		map<unsigned int, MessageNameKey > events;

	};
}
