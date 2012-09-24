#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")
#include <string>

namespace EDRendererD3D
{
	class RenderModel;
	/// Base class for lights.
	/// All types of lights should inherit from this base class.
	class Light
	{
	protected:
		/// The name of the light.  This much match the name of the light variable in the shader.
        std::string name;
		/// The color of the light to apply to the diffuse material.
		D3DXVECTOR4 diffuseColor;
		/// The light color applied regardless of lighting equation.
		D3DXVECTOR4 ambientColor;
		/// The color of the light to apply where there is a specualr reflection.
		D3DXVECTOR4 specularColor;
		/// Controls how big the specular reflection is.  A smaller value creates a large reflection.
		float specularPower;
		/// Controls the brightness of the specualr reflection.
        float specularIntensity;
		/// Disabled lights are not applied to fragments.
		bool isEnabled;

		/// Model to represent the area lit by this light.  
		/// This is required for deferred rendering.
		RenderModel *lightModelPtr;

    public:
		Light() : lightModelPtr(0) {}
		virtual ~Light();

		/// Applies the light on a given effect.
		/// This method should be called from derived versions to apply base properties.
		/// This method will set all the color, specualr properties and enabled state of the shader parameters.
		/// \param effectPtr - The effect this light should be applied to.
		virtual void ApplyLight(ID3DXEffect	*effectPtr) = 0;

		/// Sets the name of the light, used to decide what shader declared light this applies to.
        void SetName(const char *_name) { name = _name; }
        void SetDiffuseColor(const D3DXVECTOR4 *_diffuseColor) { diffuseColor = *_diffuseColor; }
        void SetAmbientColor(const D3DXVECTOR4 *_ambientColor) { ambientColor = *_ambientColor; }
        void SetSpecularColor(const D3DXVECTOR4 *_specularColor) { specularColor = *_specularColor; }
        void SetEnabled(bool _isEnabled) { isEnabled = _isEnabled; }
		void SetSpecularPower(float _specularPower) { specularPower = _specularPower; }
        void SetSpecularIntensity(float _specularIntensisty) { specularIntensity = _specularIntensisty; }

		/// \return Returns a reference to the matrix for this light.  This matrix can be used to change the
		/// matrix hierarchy, orientation and position.  The matrix returned is the one applied to the
		/// light's geometry if rendered
		D3DXMATRIX *GetWorldMatrixPtr();

		inline RenderModel *GetRenderModel() { return lightModelPtr; }

		void AddToContextSet();

		/// Method to load the model representing this light shape.
        virtual void LoadModel(const char *fileName);
	};
}