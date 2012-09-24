#pragma once

#include "RenderableBehaviorT.h"
#include "../EDRendererD3D/RenderShapeBase.h"

using namespace EDGOCA;
using namespace EDRendererD3D;

namespace Behaviors
{
	class Renderable : public RenderableBehaviorT<Renderable>
	{
	public:
		Renderable(void){}
		
		static void PopulateMessageMap(void);

		const AttributeNameKey* GetRequiredAttributes(void);

		static void OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg );
	};
}