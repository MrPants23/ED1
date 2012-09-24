#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/RenderShape.h"

using namespace EDGOCA;
using namespace EDRendererD3D;

namespace Behaviors
{
	class SkyBoxBehavior : public BehaviorT<SkyBoxBehavior>
	{
	public:
		SkyBoxBehavior(void){}
		~SkyBoxBehavior() {	}
		
		static void PopulateMessageMap(void);
		static void OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg );
	};
}