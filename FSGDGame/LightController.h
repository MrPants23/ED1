#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/Light.h"
#include "../EDRendererD3D/RenderContext.h"

using namespace EDGOCA;
using namespace EDRendererD3D;

namespace Behaviors
{
	class LightController : public BehaviorT<LightController>
	{
	public:
		LightController(void) : lightPtr(0) {}
		~LightController(void) {}

		static void PopulateMessageMap(void);
		static void OnSetLight( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnSetRenderContext( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg );
	private:
		Light *lightPtr;
		RenderContext *contextPtr;
	};
}
