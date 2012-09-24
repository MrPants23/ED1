#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

#include "RenderTexture.h"

namespace EDRendererD3D
{
    /// An object for defining a texture target for rendering.
	class CubeRenderTarget 
    {
    private:
        /// The texture to render to.
        RenderTexture *textureTargetPtr;

        /// The surface of the render texture to use.
		static const int NUM_SURFACES = 6;
        LPDIRECT3DSURFACE9 surfacePtrs[NUM_SURFACES];

        /// The surface that was valid before activating this one.
    	LPDIRECT3DSURFACE9 oldRenderTargetPtr;

        /// The surface index this render target is using.
        DWORD targetNo;
    public:
        CubeRenderTarget();
        ~CubeRenderTarget();

        /// Initializes this render target.
        /// \param size - The size of the render target.
        /// \param format - The pixel format for this render target.
        void Create(UINT size, D3DFORMAT format = D3DFMT_A2R10G10B10);

        /// Sets this as the current render target.
        /// \param _targetNo - The index of the surface this render target 
        /// should replace.  0 = COLOR0, 1 = COLOR1, etc.
        /// \param _surfaceNo - The index of the surface of the cube map to 
        /// use
        void ActivateTarget(DWORD _targetNo, DWORD _surfaceNo);

        /// Reverts back to whatever target was valid before this 
        /// one was activated.
        void RevertTarget();

        inline RenderTexture *GetTexture() { return textureTargetPtr; }
    };
}