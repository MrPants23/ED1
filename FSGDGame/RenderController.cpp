#include "RenderController.h"
#include "WorldData.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/ViewPortManager.h"
using namespace EDRendererD3D;


#include "../EDUtilities/PrintConsole.h"
#include "../EDRendererD3D/DebugRenderer.h"
using namespace EDUtilities;

#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
using namespace EDInput;

#define _USE_MATH_DEFINES
#include <math.h>

RenderController* RenderController::instancePtr = 0;

void OnRenderDepthCallback(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_Depth);
}
void OnRenderDiffuseCallback(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_Diffuse);
}
void OnRenderNormalCallback(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_Normal);
}
void OnRenderSpecularCallback(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_Specular);
}
void OnRenderDirShadowCallback(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->ToggleDebugRender();
	RenderController::GetInstance()->SetDebugBufferToRender(RenderController::GB_DirShadow);
}


RenderController::RenderController() : m_DirectionalLight(&m_DirViewProjection)
{
	m_pTransparentContext = 0;
	m_pPointDSShadow = 0;
	m_pDirectionalDSShadow = 0;
	m_pSpotDSShadow = 0;
	m_pDSGBuffers = 0;
	screenQuadMeshPtr = 0;
	shouldRenderDebugBuffer = false;
	debugBufferToRender = 0;
	VisibilityQueryInterface::theShapePtr = 0;
}

RenderController::~RenderController()
{
	ClearPostProcessChain();

	if(m_pPointDSShadow)
	{
		m_pPointDSShadow->Release();
		m_pPointDSShadow = 0;
	}
	if(m_pDirectionalDSShadow)
	{
		m_pDirectionalDSShadow->Release();
		m_pDirectionalDSShadow = 0;
	}
	if(m_pSpotDSShadow)
	{
		m_pSpotDSShadow->Release();
		m_pSpotDSShadow = 0;
	}
	if(m_pDSGBuffers)
	{
		m_pDSGBuffers->Release();
		m_pDSGBuffers = 0;
	}	
	delete screenQuadMeshPtr;

	delete VisibilityQueryInterface::theShapePtr;
}

void RenderController::Initialize(UINT viewPorts)
{
	CreateContexts();
	CreateLights();
	CreateGBuffers();
	CreateDepthBuffers();
	CreateSceneTargetsAndViewPorts(viewPorts);
	CreatePostProcessEffects();
	InitializeConsoleCommands();

	// Set up an initial shape to be used for visibility queries
	ContentHandle<RenderForm> occlusionFormHandle = ContentManager::GetInstance()->LoadXML<RenderForm>(
		"GDForm/unitcube/pCube1/pCubeShape1/frm_Occlusion_Box.xml" );
	VisibilityQueryInterface::theShapePtr = occlusionFormHandle.GetContent()->GetNewRenderShape();
	vector< ContentHandle<RenderForm> > formHandles;
	formHandles.push_back(occlusionFormHandle);
	VisibilityQueryInterface::theShapePtr->Initialize(formHandles);

	InputManager::GetReference().AddKeyTrigger('1', INPUT_PRESSED, "RenderDepths");
	EventManager::GetInstance()->RegisterForEvent("RenderDepths", 0, OnRenderDepthCallback);
	InputManager::GetReference().AddKeyTrigger('2', INPUT_PRESSED, "RenderDiffuse");
	EventManager::GetInstance()->RegisterForEvent("RenderDiffuse", 0, OnRenderDiffuseCallback);
	InputManager::GetReference().AddKeyTrigger('3', INPUT_PRESSED, "RenderNormals");
	EventManager::GetInstance()->RegisterForEvent("RenderNormals", 0, OnRenderNormalCallback);
	InputManager::GetReference().AddKeyTrigger('4', INPUT_PRESSED, "RenderSpecular");
	EventManager::GetInstance()->RegisterForEvent("RenderSpecular", 0, OnRenderSpecularCallback);
	InputManager::GetReference().AddKeyTrigger('5', INPUT_PRESSED, "RenderShadow");
	EventManager::GetInstance()->RegisterForEvent("RenderShadow", 0, OnRenderDirShadowCallback);
}

void RenderController::CreateContexts()
{
	m_2DContext.CreateEffect(L"2DPassThrough.fx");
	m_2DContext.CreateRenderSet(true, RenderSetSorted::SortReturnTrue);
	m_2DContext.RenderFunc = RenderContext::ContextPositionUVRenderFunc;
	m_2DContext.GetEffect()->SetTechnique("WithTransform");

	m_DirectionDeferredContextHandle = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_DirectionalLightDeferred.xml");
	m_DirectionDeferredContextHandle.GetContent()->SetRenderStage( RenderContext::RS_DIR_LIGHT );

	m_PointLightContextHandle = ContentManager::LoadXML<RenderContext>( "GDRenderContext/rcx_PointLightDeferred.xml" );
	m_PointLightContextHandle.GetContent()->SetRenderStage( RenderContext::RS_PNT_LIGHT );

	m_SpotLightContextHandle = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_SpotLightDeferred.xml" );
	m_SpotLightContextHandle.GetContent()->SetRenderStage( RenderContext::RS_SPT_LIGHT );
	
	// Load in the GUI context as no entities will reference it
	m_GUIContextHandle = ContentManager::LoadXML<RenderContext>( 
		"GDRenderContext/rcx_GUI.xml" );
	m_GUIContextHandle.GetContent()->SetRenderStage( RenderContext::RS_GUI);
	AddRenderContext(m_GUIContextHandle.GetContent());

	// Hack - This should happen as part of loading in the object
	debugLines = ContentManager::LoadXML<RenderContext>("GDRenderContext/rcx_DebugLines.xml");
	AddRenderContext(debugLines.GetContent());

	m_SharedParameterContext.CreateEffect(L"GBuffers.fxo");
	m_SharedParameterContext.CreateRenderSet();
	m_SharedParameterContext.RenderFunc = RenderContext::ContextSharedParameterFunc;
}

void RenderController::AddRenderContext(RenderContext* pContext)
{
	unsigned int pTechniqueValue = 0;
	std::list< RenderContext*>::iterator listIter;

	pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_DEPTH_SPT_LIGHT );
	pTechniqueValue |= (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_DEPTH_PNT_LIGHT0 );
	pTechniqueValue |= (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_DEPTH_DIR_LIGHT );

	for( listIter = m_ShadowCasters.begin(); listIter != m_ShadowCasters.end(); ++listIter )
	{
		if( (*listIter) == pContext )
		{
			pTechniqueValue = 0;
			break;
		}
	}
	if( pTechniqueValue != 0 )
		m_ShadowCasters.push_back( pContext );

	pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_GBUFFER );

	for( listIter = m_GBufferContexts.begin(); listIter != m_GBufferContexts.end(); ++listIter )
	{
		if( (*listIter) == pContext )
		{
			pTechniqueValue = 0;
			break;
		}
	}

	if( pTechniqueValue != 0 )
		m_GBufferContexts.push_back( pContext );

	pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_DEPTH_CAMERA );

	for( listIter = m_DepthCameraContexts.begin(); listIter != m_DepthCameraContexts.end(); ++listIter )
	{
		if( (*listIter) == pContext )
		{
			pTechniqueValue = 0;
			break;
		}
	}
	if( pTechniqueValue != 0 )
		m_DepthCameraContexts.push_back( pContext );

	pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_UNLIT );

	for( listIter = m_UnlitOpaqueContexts.begin(); listIter != m_UnlitOpaqueContexts.end(); ++listIter )
	{
		if( (*listIter) == pContext )
		{
			pTechniqueValue = 0;
			break;
		}
	}
	if( pTechniqueValue != 0 )
		m_UnlitOpaqueContexts.push_back( pContext );

	pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_GUI );

	for( listIter = m_GUIContexts.begin(); listIter != m_GUIContexts.end(); ++listIter )
	{
		if( (*listIter) == pContext )
		{
			pTechniqueValue = 0;
			break;
		}
	}
	if( pTechniqueValue != 0 )
		m_GUIContexts.push_back( pContext );

	pTechniqueValue = (unsigned int)pContext->GetRenderStageTechnique( RenderContext::RS_TRANSPARENT );

	if( pTechniqueValue != 0 )
		m_pTransparentContext = pContext;
}

void RenderController::CreateLights()
{
	m_DirectionalLight.SetName("DirLight");
	m_DirectionalLight.LoadModel(
		"GDModel/DirectionalLight/mdl_DirectionalLight.xml");
	float4x4 dirMatrix;
	dirMatrix.makeIdentity();
	dirMatrix.rotateGlobalY(D3DX_PI/2 + D3DX_PI/4);
	dirMatrix.rotateLocalX(D3DX_PI/6);
	*m_DirectionalLight.GetWorldMatrixPtr() = *(D3DXMATRIX *)&dirMatrix;
	m_DirectionalLight.SetDiffuseColor(&D3DXVECTOR4(0.85F, 0.85F, 0.7F, 1));
	m_DirectionalLight.SetAmbientColor(&D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1));
	m_DirectionalLight.SetEnabled(true);
	m_DirectionalLight.SetSpecularColor(&D3DXVECTOR4(1, 1, 1, 1));
	m_DirectionalLight.SetSpecularIntensity(3.0f);
	m_DirectionalLight.SetSpecularPower(36.0f);
	m_DirectionalLight.CreateRenderTarget(theDirectionalShadowSize, theDirectionalShadowSize);

	//PointLightWithShadow pointLight;
	//m_PointLights.push_back(pointLight);
	//m_PointLights.back().LoadModel( "GDModel/PointLight/mdl_PointLight.xml" );
	//m_PointLights.back().SetLightName("PointLight0");
	//m_PointLights.back().SetDiffuseColor(&D3DXVECTOR4(0, 0, 1, 1));
	//m_PointLights.back().SetSpecularColor(&D3DXVECTOR4(1, 1, 1, 1));
	//m_PointLights.back().SetAmbientColor(&D3DXVECTOR4(0.1f, 0.1f, 0.1f, 1));
	//m_PointLights.back().SetEnabled(true);
	//m_PointLights.back().SetAttenuation(&D3DXVECTOR3(0.0f, 0.5f, 0.035f));
	//m_PointLights.back().SetSpecularIntensity(1.0f);
	//m_PointLights.back().SetSpecularPower(66.0f);
	//m_PointLights.back().CreateRenderTarget(thePointShadowSize);

	//SpotLightWithShadow spotLight;
	//m_SpotLights.push_back(spotLight);
	//m_SpotLights.back().SetLightName("SpotLight0");
	//m_SpotLights.back().LoadModel( "GDModel/SpotLight/mdl_SpotLight.xml" );
	//m_SpotLights.back().SetCutOff(.8f);
	//m_SpotLights.back().SetExponent(1);
	//m_SpotLights.back().SetDiffuseColor(&D3DXVECTOR4(1, 0, 0, 1));
	//m_SpotLights.back().SetAmbientColor(&D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1));
	//m_SpotLights.back().SetEnabled(true);
	//m_SpotLights.back().SetAttenuation(&D3DXVECTOR3(1, 0.0f, 0.0125f));
	//m_SpotLights.back().SetSpecularColor(&D3DXVECTOR4(1, 1, 1, 1));
	//m_SpotLights.back().SetSpecularIntensity(3.0f);
	//m_SpotLights.back().SetSpecularPower(36.0f);
	//m_SpotLights.back().CreateRenderTarget(theSpotShadowSize, theSpotShadowSize);
}

void RenderController::CreateGBuffers()
{
	LPDIRECT3DSURFACE9 pOldRT;
	D3DSURFACE_DESC desc;
	Renderer::theDevicePtr->GetRenderTarget(0, &pOldRT );
	pOldRT->GetDesc(&desc);

	if(pOldRT)
	{
		pOldRT->Release();
		pOldRT = 0;
	}

	m_GDepthRenderTarget.Create(desc.Width,
		desc.Height, D3DFMT_R32F);

	m_GDiffuseRenderTarget.Create(desc.Width,
		desc.Height, D3DFMT_A8R8G8B8);

	m_GNormalRenderTarget.Create(desc.Width,
		desc.Height);//, D3DFMT_G16R16);

	m_GSpecularRenderTarget.Create(desc.Width,
		desc.Height);

	m_SharedParameterContext.GetEffect()->SetTexture("diffuseGBuffer",
		m_GDiffuseRenderTarget.GetTexture()->GetHandle());
	m_SharedParameterContext.GetEffect()->SetTexture("normalGBuffer",
		m_GNormalRenderTarget.GetTexture()->GetHandle());
	m_SharedParameterContext.GetEffect()->SetTexture("depthGBuffer", 
		m_GDepthRenderTarget.GetTexture()->GetHandle());
	m_SharedParameterContext.GetEffect()->SetTexture("specularGBuffer",
		m_GSpecularRenderTarget.GetTexture()->GetHandle());

	// Occlusion testing render target
	visibilityTestTarget.Create(desc.Width, desc.Height);
}

void RenderController::CreateDepthBuffers()
{
	IDirect3DSurface9 *pOldDS = NULL;
	D3DSURFACE_DESC desc;
	Renderer::theDevicePtr->GetDepthStencilSurface( &pOldDS );
	pOldDS->GetDesc(&desc);
	pOldDS->Release();

	Renderer::theDevicePtr->CreateDepthStencilSurface( thePointShadowSize, 
		thePointShadowSize, desc.Format, D3DMULTISAMPLE_NONE, 0, TRUE, 
		&m_pPointDSShadow, NULL );

	Renderer::theDevicePtr->CreateDepthStencilSurface( theDirectionalShadowSize, 
		theDirectionalShadowSize, desc.Format,D3DMULTISAMPLE_NONE, 0, TRUE, 
		&m_pDirectionalDSShadow, NULL );

	m_SpotDepths.Create(theSpotShadowSize, theSpotShadowSize);
	Renderer::theDevicePtr->CreateDepthStencilSurface( theSpotShadowSize, 
		theSpotShadowSize, desc.Format,D3DMULTISAMPLE_NONE, 0, TRUE, 
		&m_pSpotDSShadow, NULL );

	Renderer::theDevicePtr->CreateDepthStencilSurface(desc.Width,
		desc.Height, desc.Format,
		D3DMULTISAMPLE_NONE, 0, TRUE, &m_pDSGBuffers, NULL );
}

void RenderController::CreateSceneTargetsAndViewPorts(UINT viewPorts)
{
	LPDIRECT3DSURFACE9 pOldRT;
	D3DSURFACE_DESC desc;
	Renderer::theDevicePtr->GetRenderTarget(0, &pOldRT );
	pOldRT->GetDesc(&desc);

	if(pOldRT)
	{
		pOldRT->Release();
		pOldRT = 0;
	}

	// Set up the scene targets
	m_SceneTarget.Create(desc.Width, 
		desc.Height, D3DFMT_A8R8G8B8);
	m_SceneTarget.GetTexture()->GetHandle();
	screenQuadMeshPtr = RenderMesh::LoadXML("GDMesh/ScreenQuad/ScreenQuadShape.xml", "VERTEX_POSTEX");
	m_ScreenQuad.Initialize(screenQuadMeshPtr, &m_2DContext, 0);
	m_ScreenQuad.RenderFunc = RenderShapeTarget::IndexedPrimitiveNoTransformRenderFunc;

	// Creates data to be used for debug rendering of additional buffers
	InitializeDebugBuffers();

	float4x4 viewPortMat;
	if(1 == viewPorts)
	{
		viewPortMat.makeIdentity();
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, &m_2DContext, &m_SceneTarget,
			viewPortMat);
	}
	else if(2 == viewPorts)
	{
		viewPortMat.makeScale(1.0f, .5f, 1.0f);
		viewPortMat.translateGlobal(0.0f, 0.5f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, &m_2DContext, &m_SceneTarget,
			viewPortMat);

		viewPortMat.translateGlobal(0.0f, -1.0f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, &m_2DContext, &m_SceneTarget,
			viewPortMat);
	}
	else if(4 == viewPorts || 3 == viewPorts)
	{
		viewPortMat.makeScale(0.5f, .5f, 1.0f);
		viewPortMat.translateGlobal(-.5f, 0.5f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, &m_2DContext, &m_SceneTarget,
			viewPortMat);

		viewPortMat.translateGlobal(1.0f, 0.0f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, &m_2DContext, &m_SceneTarget,
			viewPortMat);

		viewPortMat.translateGlobal(-1.0f, -1.0f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, &m_2DContext, &m_SceneTarget,
			viewPortMat);

		viewPortMat.translateGlobal(1.0f, 0.0f, 0.0f);
		ViewPortManager::GetReference().AddViewPort(screenQuadMeshPtr, &m_2DContext, &m_SceneTarget,
			viewPortMat);
	}
}

void RenderController::CreatePostProcessEffects()
{
	m_EdgePostProcess.Initialize(screenQuadMeshPtr, L"DrawEdge.fxo");
	m_VerticalBlurPostProcess.Initialize(screenQuadMeshPtr, 
		L"GaussianBlurV.fxo");
	m_HorizontalBlurPostProcess.Initialize(screenQuadMeshPtr, 
		L"GaussianBlurH.fxo");
	m_DepthOfFieldPostProcess.Initialize(screenQuadMeshPtr, 
		L"DepthOfField.fxo");
	m_DownSampleX4PostProcess.Initialize(screenQuadMeshPtr, 
		L"DownFilterX4.fxo");
	m_DownSampleX4PostProcess.clearColor = D3DCOLOR_XRGB(255, 0, 0);
	m_UpSampleX4PostProcess.Initialize(screenQuadMeshPtr, 
		L"UpFilterX4.fxo");
	m_BrightPostProcess.Initialize(screenQuadMeshPtr, 
		L"BrightPass.fxo");
	m_VerticalBloomPostProcess.Initialize(screenQuadMeshPtr, 
		L"BloomV.fxo");
	m_HorizontalBloomPostProcess.Initialize(screenQuadMeshPtr, 
		L"BloomH.fxo");
	m_CombinePostProcess.Initialize(screenQuadMeshPtr, 
		L"Combine.fxo");

	m_ToneX2PostProcess.Initialize(screenQuadMeshPtr, 
		L"ToneCalc.fxo", "ToneX2");
	m_ToneX3PostProcess.Initialize(screenQuadMeshPtr, 
		L"ToneCalc.fxo", "ToneX3");
	m_ToneMappingPostProcess.Initialize(screenQuadMeshPtr, 
		L"ToneMapping.fxo");
	m_ColorEdgeCombinePostProcess.Initialize(screenQuadMeshPtr, 
		L"ColorEdgeCombine.fxo");

	m_WaterPostProcess.Initialize(screenQuadMeshPtr, L"Water.fx");
	m_WaterPostProcess.LoadXML("GDMaterial/Water/mat_water.xml");

	m_PostTestProcess.Initialize(screenQuadMeshPtr, L"PostTest.fx");
	// Set the sharp image input for Depth of Field
	m_DepthOfFieldPostProcess.GetEffect()->SetTexture("sharpTexture", 
		m_SceneTarget.GetTexture()->GetHandle());
	SetBlurConstants();

	// Set Depth of Field properties
	m_DepthOfFieldPostProcess.GetEffect()->SetFloat(
		"gShadowDepthOffset", 0.05f);
	m_DepthOfFieldPostProcess.GetEffect()->SetFloat(
		"gShadowDirOffset", 1.0f / (theDirectionalShadowSize*2.0f));

	SetDownSampleKernel();

	// FXAA stuff
	m_FXAAPostProcess.Initialize(screenQuadMeshPtr, 
		L"Fxaa.fx");
	//PushBackPostProcess(&m_FXAAPostProcess);
}

void RenderController::RenderGDepths()
{
	// Implement a solution for the Deferred Renderer Lab
	return RenderGDepthsSolution();
}

void RenderController::RenderGBuffers()
{
	// Implement a solution for the Deferred Renderer Lab
	return RenderGBuffersSolution();
}

void RenderController::RenderLightsAndForwardPass()
{
	// Implement a solution for the Deferred Renderer Lab
	return RenderLightsAndForwardPassSolution();
}

float CalcGaussianBlurWeight(float x)
{
	const float StandardDeviation = 1;
	const float StandardDeviationSquared = 
		StandardDeviation * StandardDeviation;

	return (1.0f / sqrt(2 * D3DX_PI * StandardDeviationSquared)) * 
		pow(float(M_E), float(-((x * x) / (2 * StandardDeviationSquared))));
}

void RenderController::SetBlurConstants()
{
	m_VerticalBlurPostProcess.GetEffect()->SetFloat("gCenterBlurWeights", CalcGaussianBlurWeight(0));

	D3DXVECTOR4 blurWeights(CalcGaussianBlurWeight(1), CalcGaussianBlurWeight(2), 
		CalcGaussianBlurWeight(3), CalcGaussianBlurWeight(4));
	m_VerticalBlurPostProcess.GetEffect()->SetVector("gBlurWeights", &blurWeights);
	//(1.0 / (Math.Sqrt(2.0 * Math.PI) * StandardDeviation) * Math.Exp(-Math.Pow(CDbl(Distance), 2.0) / (2.0 * Math.Pow(StandardDeviation, 2.0))))
}

void RenderController::SetDownSampleKernel()
{
	float width = (float)WorldData::GetScreenWidth();
	float height = (float)WorldData::GetScreenHeight();
	D3DXVECTOR4 PixelCoordsDownFilter[16] =
	{
		D3DXVECTOR4( 1.5f/width,  -1.5f/height, 0, 0),
		D3DXVECTOR4( 1.5f/width,  -0.5f/height, 0, 0),
		D3DXVECTOR4( 1.5f/width,   0.5f/height, 0, 0),
		D3DXVECTOR4( 1.5f/width,   1.5f/height, 0, 0),
		D3DXVECTOR4( 0.5f/width,  -1.5f/height, 0, 0),
		D3DXVECTOR4( 0.5f/width,  -0.5f/height, 0, 0),
		D3DXVECTOR4( 0.5f/width,   0.5f/height, 0, 0),
		D3DXVECTOR4( 0.5f/width,   1.5f/height, 0, 0),
		D3DXVECTOR4(-0.5f/width,  -1.5f/height, 0, 0),
		D3DXVECTOR4(-0.5f/width,  -0.5f/height, 0, 0),
		D3DXVECTOR4(-0.5f/width,   0.5f/height, 0, 0),
		D3DXVECTOR4(-0.5f/width,   1.5f/height, 0, 0),
		D3DXVECTOR4(-1.5f/width,  -1.5f/height, 0, 0),
		D3DXVECTOR4(-1.5f/width,  -0.5f/height, 0, 0),
		D3DXVECTOR4(-1.5f/width,   0.5f/height, 0, 0),
		D3DXVECTOR4(-1.5f/width,   1.5f/height, 0, 0),
	};

	m_SharedParameterContext.GetEffect()->SetVectorArray(
		"TexelCoordsDownFilterX4", PixelCoordsDownFilter, 16);
}

void RenderController::Render()
{
	D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 1, 0, 0), L"Render started yo!");
	LPDIRECT3DSURFACE9 pOldDS = NULL;

	ViewPortManager &viewPortMan = ViewPortManager::GetReference();
	size_t numViewPorts = viewPortMan.GetNumViewPorts();
	for(size_t currentPort = 0; currentPort < numViewPorts; ++currentPort)
	//for(int currentPort = numViewPorts-1; currentPort > -1; --currentPort)
	{
		// Set all shared effect data
		viewPortMan.SetActiveViewPort((unsigned int)currentPort);

		m_SharedParameterContext.RenderProcess();

		Renderer::theDevicePtr->GetDepthStencilSurface( &pOldDS );
		Renderer::theDevicePtr->SetDepthStencilSurface( m_pDSGBuffers );
		Renderer::Clear(0,0, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 0, 255), 1, 0); 

		// Deferred rendering
		RenderGDepths();
		RenderGBuffers();
		RenderLightsAndForwardPass();

		// Revert to the default depth/stencil buffer
		if( pOldDS )
		{
			Renderer::theDevicePtr->SetDepthStencilSurface( pOldDS );
			pOldDS->Release();
		}
		D3DPERF_BeginEvent(D3DCOLOR_ARGB(1, 1, 0, 0), L"Renderin' post-processes started yo!");

		// Run any post-process effects
		list< PostProcessChain * >::iterator processIter = 
			m_PostProcessChains.begin();
		for( ; processIter != m_PostProcessChains.end(); ++processIter)
			(*processIter)->Run(m_SceneTarget);
		D3DPERF_EndEvent();

		// Solution function for viewing debug buffers
		RenderDebugBuffers();

		viewPortMan.RenderActiveViewPort();
	    Renderer::IncrementFrameCounter();
	}

	// GUI elements need rendering too
	std::list< RenderContext*>::iterator guiIter = 
		m_GUIContexts.begin();
	for(; guiIter != m_GUIContexts.end(); ++guiIter)
	{
		(*guiIter)->SetRenderStage( RenderContext::RS_GUI );
		(*guiIter)->RenderProcess();
	}

	// Clear render sets
	list<RenderContext *>::iterator contextIter;

	for( contextIter = m_ShadowCasters.begin(); contextIter != m_ShadowCasters.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	for( contextIter = m_GBufferContexts.begin(); contextIter != m_GBufferContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	for( contextIter = m_DepthCameraContexts.begin(); contextIter != m_DepthCameraContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	for( contextIter = m_UnlitOpaqueContexts.begin(); contextIter != m_UnlitOpaqueContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();
	
	for( contextIter = m_GUIContexts.begin(); contextIter != m_GUIContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	if( m_pTransparentContext != 0 )
		m_pTransparentContext->ClearRenderSet();

	visiblePointLightsWithShadow.clear();
	visibleSpotLightsWithShadow.clear();

	D3DPERF_EndEvent();
}

void RenderController::RenderDirectionalDepths()
{
	// Change the stencil buffer surface
	LPDIRECT3DSURFACE9 pOldDS = NULL;
	Renderer::theDevicePtr->GetDepthStencilSurface( &pOldDS );
	Renderer::theDevicePtr->SetDepthStencilSurface( m_pDirectionalDSShadow );

	m_DirectionalLight.ActivateShadowTarget(m_SharedParameterContext.GetEffect(), 0);

	Renderer::theDevicePtr->Clear(0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(255, 255, 255), 1, 0); 

	RenderShadowCasters(RenderContext::RS_DEPTH_DIR_LIGHT);

	m_DirectionalLight.RevertTarget();
	// Change back the stencil buffer surface
	if( pOldDS )
	{
		Renderer::theDevicePtr->SetDepthStencilSurface( pOldDS );
		pOldDS->Release();
	}
}

void RenderController::RenderPointDepths(PointLightWithShadow *lightPtr, ID3DXEffect *effectPtr)
{
	// Change the stencil buffer surface
	LPDIRECT3DSURFACE9 pOldDS = NULL;
	Renderer::theDevicePtr->GetDepthStencilSurface( &pOldDS );
	Renderer::theDevicePtr->SetDepthStencilSurface( m_pPointDSShadow );

	for(UINT index = 0; index < 6; ++index)
	{
		lightPtr->ActivateShadowTarget(
			effectPtr, 0, index);

		Renderer::theDevicePtr->Clear(0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
			D3DCOLOR_XRGB(255, 255, 255), 1, 0);

		// Render depths to the texture
		RenderShadowCasters(RenderContext::RenderStage(RenderContext::RS_DEPTH_PNT_LIGHT0+index));
		lightPtr->RevertTarget();
	}
	// Change back the stencil buffer surface
	if( pOldDS )
	{
		Renderer::theDevicePtr->SetDepthStencilSurface( pOldDS );
		pOldDS->Release();
	}
}

void RenderController::RenderSpotDepths(SpotLightWithShadow *lightPtr, ID3DXEffect *effectPtr)
{
	// Change the stencil buffer surface
	LPDIRECT3DSURFACE9 pOldDS = NULL;
	Renderer::theDevicePtr->GetDepthStencilSurface( &pOldDS );
	Renderer::theDevicePtr->SetDepthStencilSurface( m_pSpotDSShadow );

	lightPtr->ActivateShadowTarget(effectPtr, 0);

	Renderer::theDevicePtr->Clear(0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(255, 255, 255), 1, 0);

	// Render depths to the texture
	RenderShadowCasters(RenderContext::RS_DEPTH_SPT_LIGHT);
	lightPtr->RevertTarget();
	
	// Change back the stencil buffer surface
	if( pOldDS )
	{
		Renderer::theDevicePtr->SetDepthStencilSurface( pOldDS );
		pOldDS->Release();
	}
}

void RenderController::RenderCameraDepths()
{
	for(list<RenderContext *>::iterator iter = 
		m_DepthCameraContexts.begin(); iter != m_DepthCameraContexts.end(); ++iter)
	{
		(*iter)->SetRenderStage( RenderContext::RS_DEPTH_CAMERA );
		(*iter)->RenderProcess();
	}
}

void RenderController::RenderShadowCasters(RenderContext::RenderStage stage)
{
	for(list<RenderContext *>::iterator iter = 
		m_ShadowCasters.begin(); iter != m_ShadowCasters.end(); ++iter)
	{
		(*iter)->SetRenderStage(stage);
		(*iter)->RenderProcess();
	}
}

void RenderController::BuildInitialiDirMatrices(const FrustumHull &frustumHull)
{
	return BuildInitialiDirMatricesSolution(frustumHull);
}

void RenderController::BuildDirViewProjection(const FrustumHull &frustumHull)
{
	return BuildDirViewProjectionSolution(frustumHull);
}

void RenderController::PushBackPostProcess(PostProcess *process)
{
	if(m_PostProcessChains.empty())
	{
		PostProcessChain *newChain = new PostProcessChain;
		newChain->Initialize();
		m_PostProcessChains.push_back(newChain);
	}
	PostProcessChain *lastChain = m_PostProcessChains.back();
	lastChain->AddPostProcess(process);
}

void RenderController::RemovePostProcess(PostProcess *process)
{
	list< PostProcessChain * >::iterator processIter = 
		m_PostProcessChains.begin();

	while(processIter != m_PostProcessChains.end())
	{
		bool increment = true;
		if(0 == (*processIter)->RemovePostProcess(process))
		{
			// Remove this empty chain
			list< PostProcessChain * >::iterator erase = processIter;
			if(m_PostProcessChains.begin() != processIter)
			{
				// Common case where we are not removing the head
				processIter--;
				delete (*erase);
				m_PostProcessChains.erase(erase);
			}
			else 
			{
				// Special case for removing the head
				increment = false;
				delete (*erase);
				m_PostProcessChains.erase(erase);
				processIter = m_PostProcessChains.begin();
			}
		}
		if(increment)
			processIter++;
	}
}

void RenderController::InitializeAVisibilityQuery(VisibilityQueryInterface &query)
{
	query.Initialize(&visibilityTestTarget, m_pDSGBuffers);
}

void RenderController::AddVisibleLight(PointLightWithShadow* light)
{
	visiblePointLightsWithShadow.insert(light);
	//if( visiblePointLightsWithShadow.insert(light).second == true )
//		light->AddToContextSet();
}

void RenderController::AddVisibleLight(SpotLightWithShadow* light)
{
	visibleSpotLightsWithShadow.insert(light);
	//if( visibleSpotLightsWithShadow.insert(light).second == true )
//		light->AddToContextSet();
}

