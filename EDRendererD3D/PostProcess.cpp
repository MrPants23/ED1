#include "PostProcess.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDRendererD3D/IndexBuffer.h"

#include "../EDUtilities/InternalOutput.h"
using EDUtilities::InternalOutput;

namespace EDRendererD3D
{
	PostProcess::~PostProcess()
	{
		if(effectPtr)
		{
			effectPtr->Release();
			effectPtr = 0;
		}
	}

	ID3DXBuffer *PostProcess::Initialize(RenderMesh *_meshPtr, LPCWSTR effectFile, char *techniqueName)
	{
		if(effectPtr)
		{
			effectPtr->Release();
			effectPtr = 0;
		}

		meshPtr = _meshPtr;
        ID3DXBuffer *errors;
#ifdef _DEBUG
        D3DXCreateEffectFromFile(Renderer::theDevicePtr, effectFile, 0, 0, 
            D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION, Renderer::theEffectPoolPtr,
            &effectPtr, &errors);
#else
        D3DXCreateEffectFromFile(Renderer::theDevicePtr, effectFile, 0, 0, 
            0, Renderer::theEffectPoolPtr, &effectPtr, &errors);
#endif

        if (errors)
        {
			InternalOutput::GetReference().Error((char *)errors->GetBufferPointer());
        }
		else
		{
			// Initialize handles
			textureHandle = effectPtr->GetParameterByName(0, "sourceTexture");
			effectPtr->SetTechnique(techniqueName);

			D3DXHANDLE scaleHandle;
			D3DXHANDLE passHandle;
			passHandle = effectPtr->GetPass(techniqueName, 0);

			targetState = PTS_Default;
			char *szScale;
			scaleHandle = effectPtr->GetAnnotationByName(passHandle, "scale");
			if(scaleHandle)
			{
				effectPtr->GetString(scaleHandle, (LPCSTR *)&szScale);

				if(0 == strncmp(szScale, "Up", 2))
					targetState = PTS_UpScale;
				else if(0 == strncmp(szScale, "Down", 4))
					targetState = PTS_DownScale;

			}

		}
        return errors;
	}

	void PostProcess::Process(RenderTarget &destinationTarget, LPDIRECT3DBASETEXTURE9 sourceTexture)
	{
		// Implement your own solution for the Post-process lab
		return ProcessSolution(destinationTarget, sourceTexture);
	}
}