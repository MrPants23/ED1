#pragma once

#include "Light.h"

namespace EDRendererD3D
{
	/// Implementation of a directional light.
	/// A DirectionalLight should be used to represent a light that is far enough away that the 
	/// rays of the light are effectively parallel.  The Frame is used to define the direction of the light.
	class DirectionalLight : public Light
	{
    public:
		/// Applies the light on a given effect.
		/// This method should be called each frame to set the direction of the light.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight(ID3DXEffect	*effectPtr);
	};
}