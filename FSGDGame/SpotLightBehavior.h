#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/SpotLightWithShadow.h"

using namespace EDGOCA;
using namespace EDRendererD3D;

namespace Behaviors
{
	class SpotLightBehavior : public BehaviorT<SpotLightBehavior>
	{
	public:
		SpotLightBehavior(void);
		~SpotLightBehavior(void);

		void OnAdd(void);
		void OnRemove(void);

		static void PopulateMessageMap(void);
		static void OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnGetRenderableCount( IBehavior* pInvokingBehavior, IMessage* msg );
	private:
		SpotLightWithShadow* lightPtr;
	};
}