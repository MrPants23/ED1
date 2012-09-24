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
	class RenderTarget 
    {
    private:
        /// The texture to render to.
        RenderTexture *textureTargetPtr;

        /// The surface of the render texture to use.
        LPDIRECT3DSURFACE9 surfacePtrs;

        /// The surface that was valid before activating this one.
    	LPDIRECT3DSURFACE9 oldRenderTargetPtr;
    	//LPDIRECT3DSURFACE9 oldRenderTargetPtrSdjlfhasjklfas;

        /// The surface index this render target is using.
        DWORD targetNo;
    public:
        RenderTarget();
        ~RenderTarget();

        /// Initializes this render target.
        /// \param width - The width of the render target.
        /// \param length - The length of the render target.
        /// \param format - The pixel format for this render target.
        void Create(UINT width, UINT length, 
            D3DFORMAT format = D3DFMT_A2R10G10B10);

        /// Sets this as the current render target.
        /// \param _targetNo - The index of the surface this render target 
        /// should replace.  0 = COLOR0, 1 = COLOR1, etc.
        void ActivateTarget(DWORD _targetNo);

        /// Reverts back to whatever target was valid before this 
        /// one was activated.
        void RevertTarget();

        inline RenderTexture *GetTexture() { return textureTargetPtr; }


        /// Solution declarations, ignore these
        void CreateSolution(UINT width, UINT length, D3DFORMAT format);
        /// Solution declarations, ignore these
        void ActivateTargetSolution(DWORD _targetNo);
        /// Solution declarations, ignore these
        void RevertTargetSolution();
    };
}