#pragma once

#include "../EDGOCA/BehaviorT.h"
using namespace EDGOCA;

#include "../EDUtilities/float4x4.h"
using EDUtilities::float4x4;

namespace Behaviors 
{
	class CameraBehavior : public BehaviorT<CameraBehavior>
	{
	public:
		CameraBehavior(void) {}
		~CameraBehavior(void) {}

		static void PopulateMessageMap(void);
		static void OnSetViewPortIndex(IBehavior* pInvokingBehavior, IMessage* msg);
		static void OnBuildProjection(IBehavior* pInvokingBehavior, IMessage* msg);
		static void OnUpdate(IBehavior* pInvokingBehavior, IMessage* msg);

		static void OnUpdateSolution(IBehavior* pInvokingBehavior, IMessage* msg);

	private:
		UINT viewPortIndex;
		float4x4 projection;
		float nearClip;
		float farClip;
	};
}
