#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

#include "RenderTarget.h"
#include "RenderMesh.h"

namespace EDRendererD3D
{
	enum PostTargetState{ PTS_Default = 0, PTS_DownScale, PTS_UpScale };
	/// An object used to implement an image space post-process effect.
	class PostProcess
	{
	protected:
		/// The shdaer effect used by this PostProcess.
		ID3DXEffect	*effectPtr;
		/// Handle to the texture defining the image to run the post-process 
		/// effect on.
		D3DXHANDLE textureHandle;

		/// The mesh that defines the space to render this post-process effect,
		/// typically a full screen quad.
		RenderMesh *meshPtr;

		PostTargetState targetState;

		/// Ignore me!
		void ProcessSolution(RenderTarget &destinationTarget, 
			LPDIRECT3DBASETEXTURE9 sourceTexture);

	public:
		PostProcess() : effectPtr(0) 
		{ 
			clearColor = D3DCOLOR_XRGB(0, 0, 0); 
		}
		virtual ~PostProcess();
		/// Creates the effect to be used by this PostProcess.
        /// \param meshPtr - The address of the mesh to be used while rendering this shape.
		/// \param effectFile - a wide character string defining the name of the 
		/// shader to use for this effect.  A leading L makes an in place string
		/// into a wide character one, ex. L"SomeShader.fx"
		/// \return Returns a buffer to any errors that occured while compiling the shader.
		virtual ID3DXBuffer *Initialize(RenderMesh *_meshPtr, LPCWSTR effectFile, 
			char *techniqueName = "PostProcess");

		/// Executes the post-process effect.
		virtual void Process(RenderTarget &destinationTarget, LPDIRECT3DBASETEXTURE9 sourceTexture);

		inline ID3DXEffect *GetEffect() { return effectPtr; }	

		inline PostTargetState GetTargetState() { return targetState; }

		// For testing 
		D3DCOLOR clearColor;
	};
}