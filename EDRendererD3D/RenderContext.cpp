#include "RenderContext.h"
#include "RenderContextWithTexture.h"
#include "Renderer.h"
#include "VertexBufferManager.h"
#include "IndexBuffer.h"
#include "RenderMaterial.h"
#include "ViewPortManager.h"
#include <string>
#include "../EDUtilities/InternalOutput.h"

#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;

#include "../tinyxml/tinyxml.h"

namespace EDRendererD3D
{
	RenderContext::RenderContext()
	{
		effectPtr = 0;
		renderSetPtr = 0;
		m_TechniqueName = "Basic";

		m_CurrentStage = RS_UNLIT;
		m_RenderStageTechnique[RS_DEPTH_SPT_LIGHT] = ""; 
		m_RenderStageTechnique[RS_DEPTH_PNT_LIGHT0] = "";
		m_RenderStageTechnique[RS_DEPTH_PNT_LIGHT1] = "";
		m_RenderStageTechnique[RS_DEPTH_PNT_LIGHT2] = "";
		m_RenderStageTechnique[RS_DEPTH_PNT_LIGHT3] = "";
		m_RenderStageTechnique[RS_DEPTH_PNT_LIGHT4] = "";
		m_RenderStageTechnique[RS_DEPTH_PNT_LIGHT5] = "";
		m_RenderStageTechnique[RS_DEPTH_DIR_LIGHT] = ""; 
		m_RenderStageTechnique[RS_DEPTH_CAMERA] = ""; 
		m_RenderStageTechnique[RS_GBUFFER] = ""; 
		m_RenderStageTechnique[RS_SPT_LIGHT] = "";
		m_RenderStageTechnique[RS_PNT_LIGHT] = ""; 
		m_RenderStageTechnique[RS_DIR_LIGHT] = "";
		m_RenderStageTechnique[RS_UNLIT] = ""; 
		m_RenderStageTechnique[RS_TRANSPARENT] = ""; 
	}
	RenderContext::~RenderContext()
	{
		if(effectPtr)
		{
			effectPtr->Release();
			effectPtr = 0;
		}
		delete renderSetPtr;
	}

	ID3DXBuffer *RenderContext::CreateEffect(LPCWSTR effectFile)
	{
		// Implement a solution for the Renderer Lab
		return CreateEffectSolution(effectFile);
	}

	void RenderContext::InitializeHandles()
	{
		// Implement a solution for the Renderer Lab
		return InitializeHandlesSolution();
	}

	void RenderContext::ContextSharedRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextSharedRenderFuncSolution(node);
	}

	void RenderContext::ContextPosNormalUVRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPosNormalUVRenderFuncSolution(node);
	}

	void RenderContext::ContextPosNormalTangentUVRenderFunc(RenderNode &node)
	{
		IDirect3DDevice9 *pDevice = Renderer::theDevicePtr;

		pDevice->SetStreamSource(0, 
			VertexBufferManager::GetReference().GetPosNormTanTexBuffer().GetVertexBuffer(),
			0, sizeof(VERTEX_POSNORMTANTEX));
		pDevice->SetVertexDeclaration(
			VertexBufferManager::GetReference().GetPosNormTanTexBuffer().GetVertexDeclaration());

		pDevice->SetIndices(IndexBuffer::GetReference().GetIndices());

		ContextSharedRenderFunc(node);
	}

	void RenderContext::ContextPositionRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPositionRenderFuncSolution(node);
	}

	void RenderContext::ContextPositionUVRenderFunc(RenderNode &node)
	{
		// Implement a solution for the Renderer Lab
		return ContextPositionUVRenderFuncSolution(node);
	}

	void RenderContext::ContextPositionColorRenderFunc(RenderNode &node)
	{
		return ContextPositionColorRenderFuncSolution(node);
	}

	void RenderContext::ContextSharedParameterFunc(RenderNode &node)
	{
		RenderContext &context = (RenderContext &)node;

		D3DXVECTOR3 pos(ViewPortManager::GetReference().GetActiveViewPosition().v);
		context.GetEffect()->SetVector(context.cameraPositionHandle, 
			&D3DXVECTOR4(pos, 1));

		context.GetEffect()->SetMatrix(
			context.viewMatrixHandle, (D3DXMATRIX *)&ViewPortManager::GetReference().GetActiveView());

		float fFar = ViewPortManager::GetReference().GetActiveViewFarClip();
		float fNear = ViewPortManager::GetReference().GetActiveViewNearClip();

		context.GetEffect()->SetFloat(
			"gNearDist", fNear);
		context.GetEffect()->SetFloat(
			"gFarDist", fFar);
		context.GetEffect()->SetFloat(
			"gPlaneX", -fFar / (fFar - fNear));
		context.GetEffect()->SetFloat(
			"gPlaneY", -fFar * fNear / (fFar - fNear));

		LPDIRECT3DSURFACE9 pOldRT;
		D3DSURFACE_DESC desc;
		Renderer::theDevicePtr->GetRenderTarget(0, &pOldRT );
		pOldRT->GetDesc(&desc);
		// Set the distance between a texel and pixel
		context.GetEffect()->SetVector("gHalfPixel", 
			&D3DXVECTOR4(0.5f / desc.Width, 0.5f / desc.Height, 0, 1));

		if(pOldRT)
		{
			pOldRT->Release();
			pOldRT = 0;
		}
		// Set the inverse view projection
		D3DXMATRIX invViewProj;
		D3DXMatrixInverse(&invViewProj, 0, (D3DXMATRIX *)&ViewPortManager::GetReference().GetActiveViewProjection());
		context.GetEffect()->SetMatrix(
			context.invViewProjMatrixHandle, &invViewProj);
	}

	RenderContext* RenderContext::LoadXML(const char* xmlFileNamePtr, const char* indentifier)
	{
		TiXmlDocument doc((ContentManager::theContentPath + xmlFileNamePtr).c_str());

		if(!doc.LoadFile())
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file exist?\n", xmlFileNamePtr);
			return 0;
		}

		TiXmlHandle hDoc(&doc);
		TiXmlElement* elementPtr;
		TiXmlHandle hRoot(0);

		elementPtr = hDoc.FirstChildElement().Element();
		if( !elementPtr )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, is the file empty?\n", xmlFileNamePtr);
			return 0;
		}

		std::string rootName = elementPtr->Value();

		if( strcmp(rootName.c_str(), "GDRenderContext") != 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a GDRenderContext element\n", 
				xmlFileNamePtr);
			return 0;
		}

		TiXmlElement* pMaterialElement = elementPtr->FirstChildElement("Material");

		std::string materialString;
		std::string effectString;
		std::string techniqueString;
		std::string vertexFormatString;
		std::string castShadowString;
		std::string renderStyleString;

		if( pMaterialElement != 0 )
		{
			if(pMaterialElement->GetText() != 0)
				materialString = pMaterialElement->GetText();
		}

		TiXmlElement* pEffectElement = elementPtr->FirstChildElement("Effect");

		if( pEffectElement == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing an Effect element\n", xmlFileNamePtr);
			return 0;
		}

		effectString = pEffectElement->GetText(); 

		TiXmlElement* pTechniqueElement = elementPtr->FirstChildElement("Technique");

		if( pTechniqueElement != 0 )
		{
			if( pTechniqueElement->GetText() != 0 )
				techniqueString = pTechniqueElement->GetText();
		}

		if( techniqueString.length() == 0 )
			techniqueString = "Basic";

		TiXmlElement* pVertexFormatElement = elementPtr->FirstChildElement("VertexFormat");

		if( pVertexFormatElement == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a VertexFormat element\n", xmlFileNamePtr);
			return 0;
		}

		vertexFormatString = pVertexFormatElement->GetText();

		//TiXmlElement* pCastShadowElement = elementPtr->FirstChildElement("CastShadows");

		//if( pCastShadowElement == 0 )
		//	return 0;

		//castShadowString = pCastShadowElement->GetText();

		//TiXmlElement* pRenderStyleElement = elementPtr->FirstChildElement("RenderStyle");

		//if( pRenderStyleElement == 0 )
		//	return 0;

		//renderStyleString = pRenderStyleElement->GetText();

		bool isDeferredPointLightContext = false;
		TiXmlElement* pPointLightTechnique = elementPtr->FirstChildElement("PointLightTechnique");
		if( pPointLightTechnique != 0 )
		{
			if( pPointLightTechnique->GetText() != 0 )
			{
				std::string technique = pPointLightTechnique->GetText();
				if( technique.length() != 0 )
					isDeferredPointLightContext = true;
			}

		}	
		bool isDeferredDirLightContext = false;
		TiXmlElement* pDirLightTechnique = elementPtr->FirstChildElement("DirLightTechnique");
		if( pDirLightTechnique != 0 )
		{
			if( pDirLightTechnique->GetText() != 0 )
			{
				std::string technique = pDirLightTechnique->GetText();
				if( technique.length() != 0 )
					isDeferredDirLightContext = true;
			}
		}
		bool isDeferredSpotLightContext = false;
		TiXmlElement* pSpotLightTechnique = elementPtr->FirstChildElement("SpotLightTechnique");
		if( pSpotLightTechnique != 0 )
		{
			if( pSpotLightTechnique->GetText() != 0 )
			{
				std::string technique = pSpotLightTechnique->GetText();
				if( technique.length() != 0 )
					isDeferredSpotLightContext = true;
			}
		}
		RenderContext* pRenderContext = 0;

		if( materialString.length() != 0 )
		{
			RenderContextWithTexture* pRcWithTex = new RenderContextWithTexture;
			pRenderContext = pRcWithTex;

			if( strcmp( vertexFormatString.c_str(), "VERTEX_POS" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPositionRenderFunc;
				pRcWithTex->m_VertexFormat = eVERTEX_POS;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSCOLOR" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPositionRenderFunc;
				pRcWithTex->m_VertexFormat = eVERTEX_POSCOLOR;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSTEX" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPositionUVRenderFunc;
				pRcWithTex->m_VertexFormat = eVERTEX_POSTEX;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTEX" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPosNormalUVRenderFunc;
				pRcWithTex->m_VertexFormat = eVERTEX_POSNORMTEX;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTANTEX" ) == 0 )
			{
				pRcWithTex->RenderFunc = RenderContextWithTexture::ContextPosNormalTangentUVRenderFunc;
				pRcWithTex->m_VertexFormat = eVERTEX_POSNORMTANTEX;
			}
		}
		else
		{
			pRenderContext = new RenderContext;

			if( strcmp( vertexFormatString.c_str(), "VERTEX_POS" ) == 0 )
			{
				pRenderContext->RenderFunc = ContextPositionRenderFunc;
				pRenderContext->m_VertexFormat = eVERTEX_POS;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSCOLOR" ) == 0 )
			{
				pRenderContext->RenderFunc = ContextPositionColorRenderFunc;
				pRenderContext->m_VertexFormat = eVERTEX_POSCOLOR;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSTEX" ) == 0 )
			{
				pRenderContext->RenderFunc = ContextPositionUVRenderFunc;
				pRenderContext->m_VertexFormat = eVERTEX_POSTEX;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTEX" ) == 0 )
			{
				pRenderContext->RenderFunc = ContextPosNormalUVRenderFunc;
				pRenderContext->m_VertexFormat = eVERTEX_POSNORMTEX;
			}
			else if( strcmp( vertexFormatString.c_str(), "VERTEX_POSNORMTANTEX" ) == 0 )
			{
				pRenderContext->RenderFunc = ContextPosNormalTangentUVRenderFunc;
				pRenderContext->m_VertexFormat = eVERTEX_POSNORMTANTEX;
			}
		}

		if( materialString.length() != 0 )
		{
			//std::string materialFilePath = "Resources/GDAssets";
			//materialFilePath += materialString;

			//ContentHandle<RenderMaterial> matHandle = ContentManager::LoadXML<RenderMaterial>(materialFilePath.c_str());
		}

		TiXmlElement* pGBufferTechnique = elementPtr->FirstChildElement("GBufferTechnique");
		if( pGBufferTechnique != 0 )
		{
			if( pGBufferTechnique->GetText() != 0 )
			{
				std::string technique = pGBufferTechnique->GetText();
				if( technique.length() != 0 )
					pRenderContext->SetRenderStageTechnique( RenderContext::RS_GBUFFER, technique.c_str() );
			}
		}

		TiXmlElement* pCameraDepthTechnique = elementPtr->FirstChildElement("DepthCameraTechnique");
		if( pCameraDepthTechnique != 0 )
		{
			if( pCameraDepthTechnique->GetText() != 0 )
			{
				std::string technique = pCameraDepthTechnique->GetText();
				if( technique.length() != 0 )
					pRenderContext->SetRenderStageTechnique( RenderContext::RS_DEPTH_CAMERA, technique.c_str() );
			}
		}

		TiXmlElement* pDepthSpotLightTechnique = elementPtr->FirstChildElement("DepthSpotLightTechnique");
		if( pDepthSpotLightTechnique != 0 )
		{
			if( pDepthSpotLightTechnique->GetText() != 0 )
			{
				std::string technique = pDepthSpotLightTechnique->GetText();
				if( technique.length() != 0 )
					pRenderContext->SetRenderStageTechnique( RenderContext::RS_DEPTH_SPT_LIGHT, technique.c_str() );
			}
		}

		TiXmlElement* pDepthPointLightTechnique = elementPtr->FirstChildElement("DepthPointLightTechnique");
		if( pDepthPointLightTechnique != 0 )
		{
			if( pDepthPointLightTechnique->GetText() != 0 )
			{
				std::string technique = pDepthPointLightTechnique->GetText();
				if( technique.length() != 0 )
				{
					for(unsigned int i = 0; i < 6; ++i)
					{
						char buffer[2];
						_itoa_s( i, buffer, 10 );
						pRenderContext->SetRenderStageTechnique( RenderContext::RenderStage(RenderContext::RS_DEPTH_PNT_LIGHT0+i), (technique + buffer).c_str() );
					}
				}
			}
		}

		TiXmlElement* pDepthDirectionalLightTechnique = elementPtr->FirstChildElement("DepthDirectionalLightTechnique");
		if( pDepthDirectionalLightTechnique != 0 )
		{
			if( pDepthDirectionalLightTechnique->GetText() != 0 )
			{
				std::string technique = pDepthDirectionalLightTechnique->GetText();
				if( technique.length() != 0 )
					pRenderContext->SetRenderStageTechnique( RenderContext::RS_DEPTH_DIR_LIGHT, technique.c_str() );
			}
		}

		TiXmlElement* pUnlitTechnique = elementPtr->FirstChildElement("UnlitTechnique");
		if( pUnlitTechnique != 0 )
		{
			if( pUnlitTechnique->GetText() != 0 )
			{
				std::string technique = pUnlitTechnique->GetText();
				if( technique.length() != 0 )
					pRenderContext->SetRenderStageTechnique( RenderContext::RS_UNLIT, technique.c_str() );
			}
		}

		TiXmlElement* pTransparentTechnique = elementPtr->FirstChildElement("TransparencyTechnique");
		if( pTransparentTechnique != 0 )
		{
			if( pTransparentTechnique->GetText() != 0 )
			{
				std::string technique = pTransparentTechnique->GetText();
				if( technique.length() != 0 )
					pRenderContext->SetRenderStageTechnique( RenderContext::RS_TRANSPARENT, technique.c_str() );
			}
		}

		TiXmlElement* pGUITechnique = elementPtr->FirstChildElement("GUITechnique");
		if( pGUITechnique != 0 )
		{
			if( pGUITechnique->GetText() != 0 )
			{
				std::string technique = pGUITechnique->GetText();
				if( technique.length() != 0 )
					pRenderContext->SetRenderStageTechnique( RenderContext::RS_GUI, technique.c_str() );
			}
		}

		if( isDeferredPointLightContext )
		{
			std::string technique = pPointLightTechnique->GetText();
			if( technique.length() != 0 )
				pRenderContext->SetRenderStageTechnique( RenderContext::RS_PNT_LIGHT, technique.c_str() );				
		}

		if( isDeferredDirLightContext )
		{
			std::string technique = pDirLightTechnique->GetText();
			if( technique.length() != 0 )
				pRenderContext->SetRenderStageTechnique( RenderContext::RS_DIR_LIGHT, technique.c_str() );				
		}

		if( isDeferredSpotLightContext )
		{
			std::string technique = pSpotLightTechnique->GetText();
			if( technique.length() != 0 )
				pRenderContext->SetRenderStageTechnique( RenderContext::RS_SPT_LIGHT, technique.c_str() );				
		}

		WCHAR *buffer = new WCHAR[effectString.length()+1];

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, effectString.c_str(), 
			static_cast<int>(effectString.length()), buffer, static_cast<int>(effectString.length()));

		buffer[effectString.length()] = 0;

		ID3DXBuffer *errors;
#ifdef _DEBUG
		D3DXCreateEffectFromFile(Renderer::theDevicePtr, buffer, 0, 0, 
			D3DXSHADER_DEBUG|D3DXSHADER_SKIPOPTIMIZATION, Renderer::theEffectPoolPtr, &pRenderContext->effectPtr, &errors);
#else
		D3DXCreateEffectFromFile(Renderer::theDevicePtr, buffer, 0, 0, 
			0, Renderer::theEffectPoolPtr, &pRenderContext->effectPtr, &errors);
#endif

		if (errors)
		{
			InternalOutput::GetReference().Error((char *)errors->GetBufferPointer());
		}
		else
		{
			pRenderContext->InitializeHandles();
		}

		//        ID3DXBuffer *errors;
		//#ifdef _DEBUG
		//        D3DXCreateEffectFromFile(Renderer::theDevicePtr, buffer, 0, 0, 
		//            D3DXSHADER_DEBUG, 0, &pRenderContext->effectPtr, &errors);
		//#else
		//        D3DXCreateEffectFromFile(Renderer::theDevicePtr, buffer, 0, 0, 
		//            0, 0, &pRenderContext->effectPtr, &errors);
		//#endif
		//
		//        if (errors)
		//            InternalOutput::GetReference() << (char *)errors->GetBufferPointer();
		//        else
		//             pRenderContext->InitializeHandles();

		delete [] buffer;

		if( pRenderContext->GetRenderStageTechnique( RenderContext::RS_TRANSPARENT ) != 0 )
			pRenderContext->CreateRenderSet( true, RenderSetSorted::ZSortSmaller );
		else
			pRenderContext->CreateRenderSet();

		pRenderContext->SetTechniqueName(techniqueString);

		return pRenderContext;
	}
}