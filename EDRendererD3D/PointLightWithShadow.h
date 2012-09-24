#pragma once

#include "PointLight.h"
#include "CubeRenderTarget.h"

namespace EDRendererD3D
{
	/// Implementation of a point light with shadow mapping data.
	/// The Frame controls the location of the light.
	class PointLightWithShadow : public PointLight
    {
    private:
        CubeRenderTarget renderTarget;

   		D3DXMATRIX projection;

		D3DXMATRIX viewProjection[6];

        D3DXMATRIX oldWorldMat;

        void UpdateMatrices();
    public:
        PointLightWithShadow();

        /// Initializes the render target.
        /// \param size - The size of the render target.
        /// \param format - The pixel format for this render target.
        void CreateRenderTarget(UINT size, D3DFORMAT format = D3DFMT_R32F);

   		/// Applies the light on a given effect.
		/// This method should be called each frame to set the light parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight(ID3DXEffect	*effectPtr);

        /// Sets the shadow target as the current render target and applies the light.
		/// \param effectPtr - The effect this light should be applied to.
        /// \param _targetNo - The index of the surface of this render target 
        /// should replace.  0 = COLOR0, 1 = COLOR1, etc.
        /// \param _surfaceNo - The index of the surface of the cube map to 
        /// use
        void ActivateShadowTarget(ID3DXEffect *effectPtr, DWORD _targetNo, DWORD _surfaceNo);

        /// Reverts back to whatever target was valid before this 
        /// one was activated.
        void RevertTarget();

    };
}