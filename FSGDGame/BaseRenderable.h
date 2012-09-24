#pragma once

#include "RenderableSetBehaviorT.h"
#include "../EDRendererD3D/RenderShapeBase.h"

using namespace EDGOCA;
using namespace EDRendererD3D;

namespace Behaviors
{
	class BaseRenderable : public RenderableSetBehaviorT<BaseRenderable>
	{
	public:
		BaseRenderable(void){}
		
		static void PopulateMessageMap(void);

		const AttributeNameKey* GetRequiredAttributes(void);

		static void OnAttributeChanged( IBehavior* pInvokingBehavior, IMessage* msg );
	};
}