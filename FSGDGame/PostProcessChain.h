#pragma once
#include "../EDRendererD3D/PostProcess.h"
#include "../EDRendererD3D/RenderTarget.h"
using namespace EDRendererD3D;

#include <list>
using std::list;

class PostProcessChain
{
private:
	/// The list of post-process effects to be run.
	list< PostProcess * > processChain;

	/// One of the two RenderTarget instances used to swap between 
	/// during effects.
	RenderTarget renderTarget1;

	/// One of the two RenderTarget instances used to swap between 
	/// during effects.
	RenderTarget renderTarget2;

	/// The render target to use when down sampling
	RenderTarget renderTargetDown1;

	/// The render target to use when up sampling
	RenderTarget renderTargetDown2;

	/// The scale to use when determining sizes for down and up sampling render targets
	UINT scale;

	/// The width of the back buffer.  This is only computed at initialization.
	static DWORD theWidth;
	/// The height of the back buffer.  This is only computed at initialization.
	static DWORD theHeight;

	/// Pointer to the RenderTarget post-process effects should use as the 
	/// source data.
	RenderTarget *sourceTargetPtr;
	
	/// Pointer to the RenderTarget to be used as the destination for 
	/// post-process effects.
	RenderTarget *destinationTargetPtr;

	void InitializeSolution();
	void RunSolution(RenderTarget &mainTarget);

public:

	PostProcessChain();

	/// Initialize the post-process effect swap chain.
	void Initialize();

	/// Run all post-process effects contained.
	/// \param mainTarget - The RenderTarget containing the original source
	/// material to be used in post.  This target will have the result of the
	/// post-process chain when this method returns.
	void Run(RenderTarget &mainTarget);

	/// Adds a post-process effect to the swap chain.
	inline void AddPostProcess(PostProcess *postProcess)
	{
		processChain.push_back(postProcess);
	}

	/// Removes a post-process effect from the swap chain.
	inline size_t RemovePostProcess(PostProcess *postProcess)
	{
		processChain.remove(postProcess);
		return processChain.size();
	}

	/// Clears the list of PostProcess pointer's contained
	inline void Clear()
	{
		processChain.clear(); 
	}

	void SwapTargets();
};