#include "RenderController.h"
#include "WorldData.h"
#include "../EDEventManager/EventManager.h"
#include "InputConsole.h"
#include "../EDLUAWrappers/SpriteWrapper.h"

float GetFloatFromString(void *stringData)
{
	char * strVal = (char*)stringData;
	return (float)atof(strVal);
}

void SetDepthOffsetCallBack(unsigned int eventId, void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetDepthOffset(val);
}

void SetFocusDistanceCallBack(unsigned int eventId, void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetFocusDistance(val);
}

void SetFocusRangeCallBack(unsigned int eventId, void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetFocusRange(val);
}

void SetEdgeIntensityCallBack(unsigned int eventId, void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetEdgeIntensity(val);
}

void SetEdgeWidthCallBack(unsigned int eventId, void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetEdgeWidth(val);
}

void SetLumGreyCallBack(unsigned int eventId, void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetLumGrey(val);
}

void SetLumWhiteCallBack(unsigned int eventId, void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetLumWhite(val);
}

void SetShadowDepthOffsetCallBack(unsigned int eventId, void *eventData, void *listener)
{
	float val = GetFloatFromString(eventData);

	RenderController::GetInstance()->SetShadowDepthOffset(val);
}

void AddDoFCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddDepthOfFieldPostProcess();
}

void AddDrawEdgesCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddDrawEdgesPostProcess();
}

void AddHorizontalBlurCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddGaussianBlurHPostProcess();
}

void AddVerticalBlurCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddGaussianBlurVPostProcess();
}

void ClearPostCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->ClearPostProcessChain();
}

void AddDownSampleCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddDownSamplePostProcess();
}

void AddUpSampleCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddUpSamplePostProcess();
}

void AddHorizontalBloomCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddBloomHPostProcess();
}

void AddVerticalBloomCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddBloomVPostProcess();
}

void AddBrightPassCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddBrightPassPostProcess();
}

void AddCombineCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddCombinePostProcess();
}

void AddBloomCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddBloomPostProcess();
}

void AddToneCalcCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddToneCalcPostProcess();
}

void AddToneMappingCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddToneMappingPostProcess();
}

void AddAntiAliasingCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddAntiAliasingPostProcess();
}

void AddFXAACallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddFXAAPostProcess();
}

void AddWaterCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddWaterPostProcess();
}

void AddPostChainCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddChainPostProcess();
}

void TestSamplingCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->TestSamplingPostProcess();
}

void AddPostTestCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->AddPostTestProcess();
}

void ReloadPostCallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->CreatePostProcessEffects();
}

void ResetGUICallBack(unsigned int eventId, void *eventData, void *listener)
{
	RenderController::GetInstance()->ResetGUI();
}

void RenderController::InitializeConsoleCommands()
{
	InputConsole &console = InputConsole::GetReference();
	// Register for events
	console.RegisterCommand("AddPostTest", 0, AddPostTestCallBack);
	console.RegisterCommand("ReloadPost", 0, ReloadPostCallBack);

	console.RegisterCommand("SetDepthOffset", 0, SetDepthOffsetCallBack);
	console.RegisterCommand("SetEdgeIntensity", 0, SetEdgeIntensityCallBack);
	console.RegisterCommand("SetEdgeWidth", 0, SetEdgeWidthCallBack);
	console.RegisterCommand("SetShadowDepthOffset", 0, SetShadowDepthOffsetCallBack);

	console.RegisterCommand("SetLumGrey", 0, SetLumGreyCallBack);
	console.RegisterCommand("SetLumWhite", 0, SetLumWhiteCallBack);

	console.RegisterCommand("AddDoF", 0, AddDoFCallBack);
	console.RegisterCommand("AddEdges", 0, AddDrawEdgesCallBack);
	console.RegisterCommand("AddBlurH", 0, AddHorizontalBlurCallBack);
	console.RegisterCommand("AddBlurV", 0, AddVerticalBlurCallBack);

	console.RegisterCommand("AddDown", 0, AddDownSampleCallBack);
	console.RegisterCommand("AddUp", 0, AddUpSampleCallBack);
	console.RegisterCommand("AddBloomH", 0, AddHorizontalBloomCallBack);
	console.RegisterCommand("AddBloomV", 0, AddVerticalBloomCallBack);
	console.RegisterCommand("AddBrightPass", 0, AddBrightPassCallBack);
	console.RegisterCommand("AddCombine", 0, AddCombineCallBack);
	console.RegisterCommand("AddBloom", 0, AddBloomCallBack);

	console.RegisterCommand("ClearPost", 0, ClearPostCallBack);
	console.RegisterCommand("SetFocusDistance", 0, SetFocusDistanceCallBack);
	console.RegisterCommand("SetFocusRange", 0, SetFocusRangeCallBack);

	console.RegisterCommand("AddToneCalc", 0, AddToneCalcCallBack);
	console.RegisterCommand("AddToneMapping", 0, AddToneMappingCallBack);

	console.RegisterCommand("AddAA", 0, AddAntiAliasingCallBack);
	console.RegisterCommand("FXAA", 0, AddFXAACallBack);

	console.RegisterCommand("AddWater", 0, AddWaterCallBack);

	console.RegisterCommand("AddPostChain", 0, AddPostChainCallBack);

	console.RegisterCommand("TestSampling", 0, TestSamplingCallBack);

	AddWaterPostProcess();

	// CS - Register GUI commands
	console.RegisterCommand("ResetGUI", 0, ResetGUICallBack);
}

void RenderController::SetDepthOffset(float val)
{
	m_SharedParameterContext.GetEffect()->SetFloat("gShadowDepthOffset", val);
}

void RenderController::SetEdgeWidth(float val)
{
	m_SharedParameterContext.GetEffect()->SetFloat("EdgeWidth", val);
}

void RenderController::SetEdgeIntensity(float val)
{
	m_SharedParameterContext.GetEffect()->SetFloat("EdgeIntensity", val);
}

void RenderController::SetFocusDistance(float val)
{
	m_DepthOfFieldPostProcess.GetEffect()->SetFloat("gFocusDistance", val);
}

void RenderController::SetFocusRange(float val)
{
	m_DepthOfFieldPostProcess.GetEffect()->SetFloat("gFocusRange", val);
}

void RenderController::SetShadowDepthOffset(float val)
{
	m_SharedParameterContext.GetEffect()->SetFloat("gShadowDepthOffset", val);
}

void RenderController::SetLumGrey(float val)
{
	m_ToneMappingPostProcess.GetEffect()->SetFloat("MIDDLE_GREY", val);
}

void RenderController::SetLumWhite(float val)
{
	m_ToneMappingPostProcess.GetEffect()->SetFloat("LUM_WHITE", val);
}

void RenderController::AddDepthOfFieldPostProcess()
{
	PostProcessChain *newChain = new PostProcessChain;
	newChain->Initialize();
	m_PostProcessChains.push_back(newChain);

	PushBackPostProcess(&m_DownSampleX4PostProcess);
	PushBackPostProcess(&m_HorizontalBlurPostProcess);
	PushBackPostProcess(&m_VerticalBlurPostProcess);
	PushBackPostProcess(&m_HorizontalBlurPostProcess);
	PushBackPostProcess(&m_VerticalBlurPostProcess);
	PushBackPostProcess(&m_UpSampleX4PostProcess);

	PushBackPostProcess(&m_DepthOfFieldPostProcess);
}

void RenderController::TestSamplingPostProcess()
{
	PushBackPostProcess(&m_DownSampleX4PostProcess);
	PushBackPostProcess(&m_UpSampleX4PostProcess);
}

void RenderController::AddDrawEdgesPostProcess()
{
	PushBackPostProcess(&m_EdgePostProcess);
}

void RenderController::AddGaussianBlurHPostProcess()
{
	PushBackPostProcess(&m_HorizontalBlurPostProcess);
}

void RenderController::AddGaussianBlurVPostProcess()
{
	PushBackPostProcess(&m_VerticalBlurPostProcess);
}

void RenderController::AddDownSamplePostProcess()
{
	PushBackPostProcess(&m_DownSampleX4PostProcess);
}

void RenderController::AddUpSamplePostProcess()
{
	PushBackPostProcess(&m_UpSampleX4PostProcess);
}

void RenderController::AddBloomHPostProcess()
{
	PushBackPostProcess(&m_HorizontalBloomPostProcess);
}

void RenderController::AddBloomVPostProcess()
{
	PushBackPostProcess(&m_VerticalBloomPostProcess);
}

void RenderController::AddCombinePostProcess()
{
	PushBackPostProcess(&m_CombinePostProcess);
}

void RenderController::AddBrightPassPostProcess()
{
	PushBackPostProcess(&m_BrightPostProcess);
}

void RenderController::AddChainPostProcess()
{
	PostProcessChain *newChain = new PostProcessChain;
	newChain->Initialize();
	m_PostProcessChains.push_back(newChain);
}

void RenderController::AddBloomPostProcess()
{
	PostProcessChain *newChain = new PostProcessChain;
	newChain->Initialize();
	m_PostProcessChains.push_back(newChain);

	PushBackPostProcess(&m_DownSampleX4PostProcess);
	PushBackPostProcess(&m_BrightPostProcess);
	PushBackPostProcess(&m_HorizontalBloomPostProcess);
	PushBackPostProcess(&m_VerticalBloomPostProcess);
	PushBackPostProcess(&m_UpSampleX4PostProcess);

	PushBackPostProcess(&m_CombinePostProcess);
}

void RenderController::AddToneCalcPostProcess()
{
	UINT width = WorldData::GetScreenWidth();
	UINT height = WorldData::GetScreenHeight();
	UINT startSize = min(width/2, height/2);

	// initial luminance calc
	PushBackPostProcess(&m_ToneX2PostProcess);

	for( UINT size = 1; size < startSize; size *= 3)
		PushBackPostProcess(&m_ToneX3PostProcess);
}

void RenderController::AddToneMappingPostProcess()
{
	PostProcessChain *newChain = new PostProcessChain;
	newChain->Initialize();
	m_PostProcessChains.push_back(newChain);

	AddToneCalcPostProcess();
	PushBackPostProcess(&m_ToneMappingPostProcess);
}

void RenderController::AddAntiAliasingPostProcess()
{
	PostProcessChain *newChain = new PostProcessChain;
	newChain->Initialize();
	m_PostProcessChains.push_back(newChain);

	PushBackPostProcess(&m_DownSampleX4PostProcess);
	PushBackPostProcess(&m_DownSampleX4PostProcess);
	PushBackPostProcess(&m_HorizontalBlurPostProcess);
	PushBackPostProcess(&m_VerticalBlurPostProcess);
	PushBackPostProcess(&m_HorizontalBlurPostProcess);
	PushBackPostProcess(&m_VerticalBlurPostProcess);
	PushBackPostProcess(&m_UpSampleX4PostProcess);
	PushBackPostProcess(&m_UpSampleX4PostProcess);

	PushBackPostProcess(&m_ColorEdgeCombinePostProcess);
}

void RenderController::AddFXAAPostProcess()
{
	PushBackPostProcess(&m_FXAAPostProcess);
}

void RenderController::AddWaterPostProcess()
{
	PostProcess & PPW = (PostProcess&)m_WaterPostProcess;
	PushBackPostProcess(&PPW);
}

void RenderController::ClearPostProcessChain()
{
	list< PostProcessChain * >::iterator processIter = 
		m_PostProcessChains.begin();
	for( ; processIter != m_PostProcessChains.end(); ++processIter)
	{	
		(*processIter)->Clear();
		delete (*processIter);
	}
	m_PostProcessChains.clear();
}

void RenderController::AddPostTestProcess()
{
	PushBackPostProcess(&m_PostTestProcess);
}

void RenderController::ResetGUI()
{
	m_GUIContextHandle.Release();
	
	list<RenderContext *, std::allocator<RenderContext *> >::iterator contextIter;
	for( contextIter = m_GUIContexts.begin(); contextIter != m_GUIContexts.end(); ++contextIter )
		(*contextIter)->ClearRenderSet();

	EDLUAWrappers::ReloadSprites();
}