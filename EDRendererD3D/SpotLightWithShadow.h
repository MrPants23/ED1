#pragma once

#include "SpotLight.h"
#include "RenderTarget.h"

namespace EDRendererD3D
{
	/// Implementation of a spot light with shadow mapping data.
	/// The Frame controls the location and orientation fo the light.
	class SpotLightWithShadow : public SpotLight
	{
	private:
		RenderTarget *renderTargetPtr;

		D3DXMATRIX projection;
	public:

        SpotLightWithShadow();
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

        RenderTarget &GetRenderTarget() { return *renderTargetPtr; }
		void SetRenderTarget(RenderTarget *_renderTargetPtr) 
		{ 
			renderTargetPtr = _renderTargetPtr; 
			D3DXMatrixPerspectiveFovLH( &projection, cutOff * 2.0f, 1, 0.1f, 
				lightModelPtr->GetShapes()[0]->GetWorldMatrixPtr()->_33 );
		}
	};
}