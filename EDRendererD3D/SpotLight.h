#pragma once

#include "Light.h"
#include "RenderModel.h"

namespace EDRendererD3D
{
	/// Implementation of a spot light.
	/// The Frame controls the location and direction of the light.
	class SpotLight : public Light
	{
	protected:
		/// Controls how wide the angle of the spot is.
		/// This is the cosine of the angle between the direction and the furthest point to be lit.
		float cutOff;
		/// Controls how the light attenuates as ig gets further from the direction.
		/// A larger value creates a faster fall off of brightness.
		float exponent;

        /// A vector representing the attenuation of the light.
        /// x = constant attenuation, y = linear attenuation, z = quadratic attenuation
        D3DXVECTOR3 attenuation;

		float radius;

    public:

		/// Applies the light on a given effect.
		/// This method should be called each frame to set the direction of the light.
		/// input : effectPtr - The effect this light should be applied to.
		virtual void ApplyLight(ID3DXEffect	*effectPtr);

        void SetCutOff(float _cutOff);
        void SetExponent(float exponent) { exponent = exponent; }
        /// Sets the all three types of attenuation.
        /// x = constant attenuation, y = linear attenuation, z = quadratic attenuation
        void SetAttenuation(const D3DXVECTOR3 *_attenuation, float _radius);

		const D3DXVECTOR3& GetAttenuation(void){ return attenuation; }
		float GetCutOff(void){ return cutOff; }
    };
}