#include "RenderSetSorted.h"
#include "RenderContext.h"
#include "RenderShape.h"

namespace EDRendererD3D
{
	void RenderSetSorted::AddRenderNode(RenderNode *nodePtr)
	{
        // Implement your own solution for the sorting and hierarchy labs
        return AddRenderNodeSolution(nodePtr);
	}

	bool RenderSetSorted::ZSortGreater(RenderShape *left, RenderShape *right)
	{
		return left->GetZDepth() > right->GetZDepth();
	}

	bool RenderSetSorted::ZSortSmaller(RenderShape *left, RenderShape *right)
	{
		return left->GetZDepth() < right->GetZDepth();
	}

	bool RenderSetSorted::SortReturnTrue(RenderShape *left, RenderShape *right)
	{
		return true;
	}

	bool RenderSetSorted::SortReturnFalse(RenderShape *left, RenderShape *right)
	{
		return false;
	}
}