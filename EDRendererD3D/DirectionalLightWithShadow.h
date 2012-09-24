#pragma once

#include "DirectionalLight.h"
#include "RenderTarget.h"

namespace EDRendererD3D
{
	/// Implementation of a point light with shadow mapping data.
	/// The Frame controls the location of the light.
	class DirectionalLightWithShadow : public DirectionalLight
    {
    private:
        RenderTarget renderTarget;

		D3DXMATRIX *viewProjection;

    public:
        DirectionalLightWithShadow(D3DXMATRIX *_ViewProjection);

		DirectionalLightWithShadow(void);
        /// Initializes the render target.
        /// \param width - The width of the render target.
        /// \param length - The length of the render target.
        /// \param format - The pixel format for this render target.
        void CreateRenderTarget(UINT width, UINT length, 
            D3DFORMAT format = D3DFMT_R32F);

   		/// Applies the light on a given effect.
		/// This method should be called each frame to set the light parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight(ID3DXEffect	*effectPtr);

        /// Sets the shadow target as the current render target 
        /// and applies the light.
		/// \param effectPtr - The effect this light should be applied to.
        /// \param _targetNo - The index of the surface of this render target 
        /// should replace.  0 = COLOR0, 1 = COLOR1, etc.
        void ActivateShadowTarget(ID3DXEffect *effectPtr, DWORD _targetNo);

        /// Reverts back to whatever target was valid before this 
        /// one was activated.
        void RevertTarget();

        RenderTarget &GetRenderTarget() { return renderTarget; }
    };
}