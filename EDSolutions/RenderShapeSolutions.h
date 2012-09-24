#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

namespace EDRendererD3D
{
    class RenderNode;

    void PrimitiveRenderFuncSolution(RenderNode &node);
    void PrimitiveNoTransformRenderFuncSolution(RenderNode &node);
    void IndexedPrimitiveRenderFuncSolution(RenderNode &node);
	void IndexedPrimitiveSubDivRenderFuncSolution(RenderNode &node);
    void IndexedPrimitiveNoTransformRenderFuncSolution(RenderNode &node);

}