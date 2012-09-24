#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/DirectionalLight.h"
#include "../EDRendererD3D/RenderContext.h"

using namespace EDGOCA;
using namespace EDRendererD3D;

namespace Behaviors
{
	class DirectionalLightController : public BehaviorT<DirectionalLightController>
	{
	public:
		DirectionalLightController(void) : lightPtr(0) {}
		~DirectionalLightController(void) {}

		static void PopulateMessageMap(void);
		static void OnSetLight( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnSetRenderContext( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg );
		static void DirectionalLightController::OnGetRenderableCount( IBehavior* pInvokingBehavior, IMessage* msg );

	private:
		DirectionalLight *lightPtr;
		RenderContext *contextPtr;
	};
}
