#include "PostProcessChain.h"
#include "WorldData.h"
#include "RenderController.h"

DWORD PostProcessChain::theWidth;
DWORD PostProcessChain::theHeight;

PostProcessChain::PostProcessChain()
{
	sourceTargetPtr = destinationTargetPtr = 0;
	scale = 2;
}

void PostProcessChain::Initialize()
{
	// Implement your own solution for the Post-process lab
	return InitializeSolution();
}

void PostProcessChain::Run(RenderTarget &mainTarget)
{
	// Implement your own solution for the Post-process lab
	RunSolution(mainTarget);

	// We need the destination of the effect chain to be written to the mainTarget before we return
	RenderContext *sceneContext =  RenderController::GetInstance()->GetScreenContextPtr();
	RenderShapeTarget *screenShape = RenderController::GetInstance()->GetScreenShapePtr();
		
	mainTarget.ActivateTarget(0);
	screenShape->SetRenderTarget(sourceTargetPtr);
	screenShape->AddToContextSet();
	sceneContext->RenderProcess();
	sceneContext->ClearRenderSet();
	mainTarget.RevertTarget();
}

void PostProcessChain::SwapTargets()
{
	RenderTarget *tempPtr = sourceTargetPtr;
	sourceTargetPtr = destinationTargetPtr;
	destinationTargetPtr = tempPtr;
}
