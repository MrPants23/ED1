#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

namespace EDRendererD3D
{
    class RenderSet;
    class RenderContext;
	/// Renderer used to render RenderNodes contained in RenderSet instances.
    class Renderer
    {
		static UINT theFrameCounter;
    public:
		Renderer(void) {}
		~Renderer(void) {}

		/// The D3D object to be used for rendering.
        static IDirect3D9 *theD3DPtr;
		/// The D3D Device to be used for rendering.  Many D3D API methods
		/// require device, this is the one to use in most cases.
        static IDirect3DDevice9 *theDevicePtr;

        static ID3DXEffectPool *theEffectPoolPtr;
		/// Creates the D3D object and device with default presentation parameters.
		/// \param hWnd - The handle to the window to attach D3D to.
        static void Initialize(HWND hWnd);
		/// Creates the D3D object and device with presentation parameters provided.
		/// \param d3dpp - A D3DPRESENT_PARAMETERS filled out with the parameters to be
		///					used while creating the D3D device.
        static void Initialize(D3DPRESENT_PARAMETERS &d3dpp);

		/// Releases the D3D object and device.
        static void Shutdown();

        /// Renders a batch of RenderNode instances contained in the RenderSet passed in.
		/// \param set - The batch to be rendered.
        static void Render(RenderSet &set);

		inline static UINT GetFrameNumber(void){ return theFrameCounter; }
		inline static void IncrementFrameCounter(void){ ++theFrameCounter; }

		/// Clears one or more surfaces
		/// \param count - The number of rectangles in the array rectsptr.  Set to nullptr if
		/// rectsptr is null.
		/// \param rectsptr - Pointer to an array of rects that describe the area(s) to be cleared.
		/// To clear the entire viewport set this to nullptr.
		/// \param flags - Combination of one or more D3DCLEAR flags that specify what should be 
		/// cleared, ex. D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL
		/// \param clearColor - The color to clear target to.
		/// \param clearDepth - A depth value to clear to ranging from 0 to 1.
		/// \param clearStencil - A stencil value to clear to ranging from 0 to 2^n-1 where 
		/// n is the bit depth of the stencil buffer.
		inline static void Clear(DWORD count, const D3DRECT *rectsptr, DWORD flags, 
			D3DCOLOR clearColor, float clearDepth, DWORD clearStencil)
		{
			theDevicePtr->Clear(count, rectsptr, flags, clearColor, clearDepth, clearStencil);
		}

		/// Begins a scene for rendering
		inline static void BeginScene(void)
		{
			theDevicePtr->BeginScene();
		}

		/// Ends a scene 
		inline static void EndScene(void)
		{
			theDevicePtr->EndScene();
		}

		/// Presents the next buffer.  Common use will set all parameters to 0.
		/// \param sourceRectPtr - Pointer to the source rectangle to be presented.  
		/// This must be null if the swap chain was not created with D3DSWAPEFFECT_COPY.
		/// If null the entire source is presented.
		/// \param destRectPtr - Pointer to the destination rectangle to be presented to.  
		/// This must be null if the swap chain was not created with D3DSWAPEFFECT_COPY.
		/// If null the entire deswtination area is presented to.
		/// \param hDestWindow - Handle to the window to present to.  If null the handle supplied
		/// when creating the device will be used.
		/// \param dirtyRegionPtr - Pointer to the a rectangle defining the smallest area 
		/// that needs to be updated.  This is a possible optimization, but cannot be relied on as 
		/// being the only area that will be cleared. 
		/// This must be null if the swap chain was not created with D3DSWAPEFFECT_COPY.
		inline static void Present( const RECT *sourceRectPtr, const RECT *destRectPtr, 
			HWND hDestWindow, const RGNDATA *dirtyRegionPtr)
		{
			theDevicePtr->Present(sourceRectPtr, destRectPtr, hDestWindow, dirtyRegionPtr);
		}

    };
}