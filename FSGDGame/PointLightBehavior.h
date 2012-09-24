#pragma once

#include "../EDGOCA/BehaviorT.h"
#include "../EDRendererD3D/PointLightWithShadow.h"

using namespace EDGOCA;
using namespace EDRendererD3D;

namespace Behaviors
{
	class PointLightBehavior : public BehaviorT<PointLightBehavior>
	{
	public:
		PointLightBehavior(void);
		~PointLightBehavior(void);

		void OnAdd(void);

		const AttributeNameKey* GetRequiredAttributes(void);

		static void PopulateMessageMap(void);
		static void OnFrustumTest( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnBuildLocalAabb( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnGetRenderableCount( IBehavior* pInvokingBehavior, IMessage* msg );
		static void OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg );
	private:
		PointLightWithShadow* lightPtr;
	};
}