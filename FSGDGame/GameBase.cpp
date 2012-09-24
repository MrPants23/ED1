//#include "StdAfx.h"
#include "GameBase.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


#include "../EDUtilities/Timer.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/IndexBuffer.h"
#include "../EDRendererD3D/ViewPortManager.h"
#include "../EDRendererD3D/VertexBufferManager.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDEventManager/EventManager.h"
#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDUtilities/InternalOutput.h"

#include "WorldData.h"
#include "InputConsole.h"
#include "RenderController.h"

using namespace EDCollision;
using namespace EDRendererD3D;
using namespace EDUtilities;

extern HWND globalHWnd;
 
GameBase::GameBase(void) 
{
	WorldData::SetScreenHeight(768);
	WorldData::SetScreenWidth(1024);
}

GameBase::~GameBase(void)
{
	DebugRenderer::DeleteInstance();
	InternalOutput::DeleteInstance();
	VertexBufferManager::DeleteInstance();
	IndexBuffer::DeleteInstance();
	InputConsole::DeleteInstance();
	PrintConsole::DeleteInstance();
	ViewPortManager::DeleteInstance();
	RenderController::DeleteInstance();
	ContentManager::DeleteInstance();
	Renderer::Shutdown();
}

void GameBase::Initialize() 
{   
	WorldData::timer.Reset();
	PrintConsole::GetReference().Initialize();

	Renderer::Initialize(globalHWnd);
	RenderController::GetInstance()->Initialize(1);
	PrintConsole::GetReference().CreateRenderFont(Renderer::theDevicePtr);
}

void GameBase::Resize(int width, int height)
{
	// update the directional light projection matrix
	RenderController::GetInstance()->BuildInitialiDirMatrices(m_Frustum);
}

void GameBase::Update()
{
	// Track frame rate
	static int nFrames = 0;
	static Timer frameTimer;
	++nFrames;

	if(50 < nFrames)
	{
		m_FPS = 50.0f / frameTimer.GetElapsedTime();
		nFrames = 0;
		frameTimer.Reset();
	}

	//Timer
	WorldData::UpdateDeltaTime();
	float fDeltaTime = WorldData::GetDeltaTime();

	PrintConsole::GetReference().AddString(PrintConsole::PPN_Render, float3(1.0f, 0.0f, 0.0f), "FPS = %f", m_FPS);

	EDMemoryManager::MemoryManager *memoryManagerInstance = EDMemoryManager::MemoryManager::GetInstance();

	size_t totalMemoryAvailable = memoryManagerInstance->GetTotalMemoryAvailable();
	size_t currentMemoryUsage = memoryManagerInstance->GetTotalMemoryUsed();
	float memoryUsagePercentage = currentMemoryUsage / (float)totalMemoryAvailable;

	PrintConsole::GetReference().AddString(PrintConsole::PPN_Memory, float3(1.0f, 0.0f, 0.0f), "Memory usage = %d/%d (%0.2f%%)", currentMemoryUsage, totalMemoryAvailable, memoryUsagePercentage);

	EDEventManager::EventManager::GetInstance()->Update();
}
