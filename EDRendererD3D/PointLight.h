#pragma once

#include "Light.h"

namespace EDRendererD3D
{
	/// Implementation of a point light.
	/// The Frame controls the location of the light.
	class PointLight : public Light
	{
	private:
		/// A vector representing the attenuation of the light.
        /// x = constant attenuation, y = linear attenuation, z = quadratic attenuation
        D3DXVECTOR3 attenuation;

        float radius;
                      
    public:
		PointLight();

		/// Applies the light on a given effect.
		/// This method should be called each frame to set the light parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight(ID3DXEffect	*effectPtr);

		/// Sets the all three types of attenuation.
		/// \param attenuation - A three component vector used to set all attenuation types
        /// x = constant attenuation, y = linear attenuation, z = quadratic attenuation
		/// \param radius - The maximum range the light should effect
        void SetAttenuation(const D3DXVECTOR3 *_attenuation, float _radius);

        inline float GetRadius() { return radius; }
		virtual void LoadModel(const char *fileName);
	};
}