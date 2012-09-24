#include "FSGDGame.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
#include "../EDInput/JoystickManager.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/ViewPortManager.h"

#include "../EDRendererD3D/DebugRenderer.h"
#include "../EDUtilities/PrintConsole.h"
#include "InputConsole.h"
#include "WorldData.h"

#include "Behaviors.h"

#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;

//CS - Lua stuff
#include "..\EDLUAWrappers\SpriteWrapper.h"
#include "../EDThreadPool/ThreadPool.h"
#include "../EDThreadPool/WorkerExample.h"

using namespace EDCollision;
using namespace EDInput;//::InputManager;
using namespace EDRendererD3D;
using namespace EDUtilities;
using namespace EDThreadPool;

extern HWND globalHWnd;
extern GameBase *globalGamePtr;

namespace
{
	DeclareMessage( MSG_MOVE_FORWARD );
	DeclareMessage( MSG_MOVE_BACKWARD );
	DeclareMessage( MSG_MOVE_LEFT );
	DeclareMessage( MSG_MOVE_RIGHT );
	DeclareMessage( MSG_SET_RENDERSHAPE );
	DeclareMessage( MSG_FRUSTUM_TEST );
	DeclareMessage( MSG_TURN_LEFT );
	DeclareMessage( MSG_TURN_RIGHT );
	DeclareMessage( MSG_SET_COLLIDER_MESH );
	DeclareMessage( MSG_APPLY_FORCE );
	DeclareMessage( MSG_MOVE_UP );
	DeclareMessage( MSG_ATTACH );
	DeclareMessage( MSG_SET_OFFSET );
	DeclareMessage(MSG_SET_PORT_INDEX);
	DeclareMessage(MSG_BUILD_PROJECTION);

	DeclareMessage( MSG_INIT_KEYS);
	DeclareMessage( MSG_INIT_MOUSE);
	DeclareMessage( MSG_INIT_JOYSTICK);
	
	DeclareMessage( MSG_SET_DIRECTIONAL_LIGHT );
	DeclareMessage( MSG_SET_DIRECTIONAL_LIGHT_CONTEXT );

	DeclareMessage( MSG_SET_LIGHT );
	DeclareMessage( MSG_SET_LIGHT_CONTEXT );
	DeclareMessage( MSG_SET_LOCAL );
	DeclareMessage( MSG_SET_TARGET );
}

Game::Game(void) 
{
	isPaused = false;
	EDThreadPool::ThreadPool::GetInstance()->SetShutdownMethod(EDThreadPool::DO_CURRENT_WORK);
}

Game::~Game(void)
{
	EDThreadPool::ThreadPool::DeleteInstance();
}

void ExitCallBack(unsigned int eventId, void *eventData, void *listener)
{
	PostQuitMessage(0);
}

void TogglePauseCallBack(unsigned int eventId, void *eventData, void *listener)
{
	((Game *)globalGamePtr)->TogglePause();
}

void ToggleActiveControllerCallBack(unsigned int eventId, void *eventData, void *listener)
{
	((Game *)globalGamePtr)->ToggleActiveController();
}

void ToggleActiveViewerCallBack(unsigned int eventId, void *eventData, void *listener)
{
	((Game *)globalGamePtr)->ToggleActiveViewer();
}
void RenderSplash(unsigned int eventId, void *eventData, void *listener)
{
	((Game*)globalGamePtr)->ExitSplashScreen();
}

void ToggleOcclusionCullingCallBack(unsigned int eventId, void *eventData, void *listener)
{
	bool state = true;
	char* param = (char*)eventData;

	if( param != 0 )
	{
		if(strcmp(param, "0") == 0)
			state = false;
		else if(_stricmp(param, "false") == 0)
			state = false;
	}

	((Game*)globalGamePtr)->SetOcclusionCullingState(state);
}

// spawns a test task for the thread pool
void PoolTaskExample(unsigned int _eventId, void* _eventData, void* _listener)
{
	if (_eventData != nullptr)
	{
		WorkerExample* worker = new WorkerExample(atoi((char*)_eventData));
		ThreadPool::GetInstance()->PostWork(worker);
		ThreadPool::GetInstance()->testObjects.push_back(worker);
	}
}

void Game::SetOcclusionCullingState(bool state)
{
	if( state == true )
		m_TestScene.EnableSceneFlags( Scene::SF_OCCLUSION_CULL );
	else
		m_TestScene.DisableSceneFlags( Scene::SF_OCCLUSION_CULL );
}

void Game::ToggleActiveViewer()
{
	//if( m_pActiveViewer == m_pCameraEntity )
	//	m_pActiveViewer = m_pRobotKid;
	//else
	//	m_pActiveViewer = m_pCameraEntity;
}
void Game::ExitSplashScreen()
{
	//if(m_pSplashScreen)
	//{
	//	m_pSplashScreen->RemoveComponent(m_pSplashScreen->GetComponent("RenderableComponent"));
	//	m_Level.GetEntityList().remove(m_pSplashScreen);
	//	delete m_pSplashScreen;
	//	m_pSplashScreen = 0;
	//}
}

void Game::TogglePause()
{
	isPaused = !isPaused;
	if(isPaused)
		pauseEffect.Activate();
	else
		pauseEffect.Deactivate();
}

void Game::ToggleActiveController()
{
	//m_pSphereController->SetEnabled(!m_pSphereController->IsEnabled());
	//m_pPlayerController->SetEnabled(!m_pSphereController->IsEnabled());
}

void Game::Initialize() 
{

	/*TODO: ADD TIMS CODE*/

	/*Alvin Edit*/

	/*Again*/

	/*ONE MORE TIME*/
	ContentManager::theContentPath = "Resources/GDAssets/";

	GameBase::Initialize();

	m_TestScene.SetOcclusionQueryCreator( FSGDOcclusionQuery::creator );

	InitTestObjects();

	// all joystick button mapping is done in this function:
	JoystickManager::GetReference().LoadFSGDGameTriggers();

	EDEventManager::EventManager::GetInstance()->RegisterForEvent("Splash",0,RenderSplash);
	EDEventManager::EventManager::GetInstance()->RegisterForEvent("Exit", 0, ExitCallBack);
	InputManager::GetReference().AddKeyTrigger(VK_ESCAPE, INPUT_PRESSED, "Exit", 
		EDInput::RAW_INPUT | EDInput::TYPED_INPUT);

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"PauseGame", 0, TogglePauseCallBack);
	InputManager::GetReference().AddKeyTrigger('P', INPUT_PRESSED, "PauseGame");

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"ToggleActiveController", 0, ToggleActiveControllerCallBack);
	InputManager::GetReference().AddKeyTrigger('Z', INPUT_PRESSED, "ToggleActiveController");

	InputManager::GetReference().SetTypingCallback(InputConsole::TypingCallback);

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"ToggleActiveViewer", 0, ToggleActiveViewerCallBack);
	InputManager::GetReference().AddKeyTrigger('V', INPUT_PRESSED, "ToggleActiveViewer");

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"TogglePrintConsole", 0, 
		(EDEventManager::EM_CALLBACK)PrintConsole::ToggleConsoleCallBack);
	InputManager::GetReference().AddKeyTrigger('O', INPUT_PRESSED, "TogglePrintConsole");

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"NextPageConsole", 0, 
		(EDEventManager::EM_CALLBACK)PrintConsole::NextPageCallBack);
	InputManager::GetReference().AddKeyTrigger('I', INPUT_PRESSED, "NextPageConsole");

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"PreviousPageConsole", 0, 
		(EDEventManager::EM_CALLBACK)PrintConsole::PreviousPageCallBack);
	InputManager::GetReference().AddKeyTrigger('U', INPUT_PRESSED, "PreviousPageConsole");

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"NextClassConsole", 0, 
		(EDEventManager::EM_CALLBACK)PrintConsole::NextClassCallBack);
	InputManager::GetReference().AddKeyTrigger('K', INPUT_PRESSED, "NextClassConsole");

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"NextLabConsole", 0, 
		(EDEventManager::EM_CALLBACK)PrintConsole::NextLabCallBack);
	InputManager::GetReference().AddKeyTrigger('L', INPUT_PRESSED, "NextLabConsole");

	InputManager::GetReference().AddKeyTrigger('W', INPUT_HELD, "MoveForward");
	InputManager::GetReference().AddKeyTrigger('S', INPUT_HELD, "MoveBackward");
	InputManager::GetReference().AddKeyTrigger('A', INPUT_HELD, "MoveLeft");
	InputManager::GetReference().AddKeyTrigger('D', INPUT_HELD, "MoveRight");
	InputManager::GetReference().AddKeyTrigger('E', INPUT_HELD, "MoveUp");
	InputManager::GetReference().AddKeyTrigger('X', INPUT_HELD, "MoveDown");
	InputManager::GetReference().AddKeyTrigger('T', INPUT_HELD, "RotateRight");
	InputManager::GetReference().AddKeyTrigger('R', INPUT_HELD, "RotateLeft");
	InputManager::GetReference().AddKeyTrigger('G', INPUT_HELD, "RotateUp");
	InputManager::GetReference().AddKeyTrigger('F', INPUT_HELD, "RotateDown");
	InputManager::GetReference().AddKeyTrigger(VK_RETURN, INPUT_PRESSED,"Splash");

	// Sets page up/down to change camera behaviors 
	InputManager::GetReference().AddKeyTrigger(VK_PRIOR, INPUT_RELEASED, "CamBehaviorPrev");
	InputManager::GetReference().AddKeyTrigger(VK_NEXT, INPUT_RELEASED, "CamBehaviorNext");

	InputManager::GetReference().AddKeyTrigger(VK_UP, INPUT_HELD, "CameraMoveForward");
	InputManager::GetReference().AddKeyTrigger(VK_DOWN, INPUT_HELD, "CameraMoveBackward");
	InputManager::GetReference().AddKeyTrigger(VK_LEFT, INPUT_HELD, "CameraMoveLeft");
	InputManager::GetReference().AddKeyTrigger(VK_RIGHT, INPUT_HELD, "CameraMoveRight");
	InputManager::GetReference().AddMouseTrigger(VK_RBUTTON, INPUT_PRESSED, "CameraMouseTurnEnable");
	InputManager::GetReference().AddMouseTrigger(VK_RBUTTON, INPUT_RELEASED, "CameraMouseTurnDisable",
		EDInput::RAW_INPUT | EDInput::TYPED_INPUT);

	InputManager::GetReference().AddKeyTrigger(VK_OEM_3, INPUT_PRESSED, "ToggleConsole", 
		EDInput::RAW_INPUT | EDInput::TYPED_INPUT);
	InputManager::GetReference().AddKeyTrigger(VK_RIGHT, INPUT_PRESSED, "CompleteConsoleString", 
		EDInput::TYPED_INPUT);
	InputManager::GetReference().AddKeyTrigger(VK_UP, INPUT_PRESSED, "RememberConsoleString", 
		EDInput::TYPED_INPUT);

	// Test pause sprite
	pauseSprite.LoadFromFile("GDModel/mdl_PauseScreen.xml", (float)WorldData::GetScreenWidth()/2,
		(float)WorldData::GetScreenHeight()/4, 100, 100);

	// print console background initialization
	consoleBackground.LoadFromFile("GDModel/mdl_ConsoleBackground.xml", (float)WorldData::GetScreenWidth()/2,
		(float)WorldData::GetScreenHeight()/8 * 3, 
		(float)WorldData::GetScreenWidth(), (float)WorldData::GetScreenHeight()/4 * 3, 
		0, 0.25f);

	pauseEffect.Initialize();

	InputConsole::GetReference().RegisterCommand("ToggleOcclusionCulling", 0, ToggleOcclusionCullingCallBack);

	InputConsole::GetReference().RegisterCommand("PoolTask", 0, PoolTaskExample);

	// CS - Load GUI sprites
	//EDLUAWrappers::LoadSprites();
}

void Game::Resize(int width, int height)
{
	// Update the cameras with the new aspect ratio
	// This data is being sent as a message to a GOCA object, 
	// the data format is perspective width, height, near, far.
	float projectionProperties[4] = { (float)width, (float)height, 0.1f, 200.0f };
	m_Frustum.Build( 0.78539816f, projectionProperties[0], projectionProperties[1],
		projectionProperties[2], projectionProperties[3]);

	size_t numCameras = cameras.size();
	for(size_t i = 0; i < numCameras; ++i)
	{
		cameras[i]->OnMessage(&MessageT<float *>(MSG_BUILD_PROJECTION, projectionProperties));
	}

	GameBase::Resize(width, height);
}

void Game::Update()
{
	GameBase::Update();

	m_TestScene.Update();
	
	// Update frustum
// TODO - do this per viewport
	FrustumHull tempFrustum = m_Frustum;
	tempFrustum.Transform( ViewPortManager::GetReference().GetActiveWorld() );
	m_TestScene.FrustumCull( &tempFrustum, &MessageT<FrustumHull*>( MSG_FRUSTUM_TEST, &tempFrustum ) );

	RenderController::GetInstance()->BuildDirViewProjection(tempFrustum);

	DebugRenderer::GetInstance()->RenderDebugPrimitives();

	// checks the status of test objects for the thread pool
	ThreadPool::GetInstance()->CheckTestObjects();
}

void Game::Render()
{
	// clear the back buffer to blue
	Renderer::Clear(0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 0, 255), 1, 0); 
	Renderer::BeginScene();

	RenderController::GetInstance()->Render();

	if(PrintConsole::GetReference().IsEnabled())
		consoleBackground.Draw();

	PrintConsole::GetReference().Render();
	InputConsole::GetReference().Render();

	if(isPaused)
	{
		pauseSprite.Draw();
	}

	//CS - Comment this back in to draw sprites
	//EDLUAWrappers::DrawSprites();
	 
	Renderer::EndScene();

	// buffer swap
	Renderer::Present(0,0,0,0);
}

void Game::InitTestObjects()
{
	float4x4 terrainMat;
	terrainMat.makeIdentity();
	for(int i = -2; i < 3; ++i)
	{
		for(int j = -2; j < 3; ++j)
		{
			terrainMat.WAxis.x = 100.0f * i;
			terrainMat.WAxis.z = 100.0f * j;
			m_TestScene.SpawnObject("GameObjects/GO_Terrain.xml", &terrainMat);
		}
	}

	m_TestScene.SpawnObject("GameObjects/GO_SkyBox.xml");

	float4x4 playerMat;
	playerMat.makeIdentity();
	playerMat.WAxis = float3( 0.0f, 0.0f, 2.0f );
	GameObject *playerPtr = m_TestScene.SpawnObject("GameObjects/GO_Buggy.xml", &playerMat);

	playerPtr->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/PlayerKeyboardMapping.xml")); 
	playerPtr->OnMessage( &MessageT<string>( MSG_INIT_JOYSTICK, "KeyMappings/PlayerJoystickMapping.xml")); 
	
	//GameObject* buggy = m_TestScene.SpawnObject("GameObjects/GO_Buggy.xml");
	//buggy->OnMessage( &MessageT<GameObject*>( MSG_SET_TARGET, playerPtr ) );
	GameObject* buggy = m_TestScene.SpawnObject("GameObjects/GO_Buggy.xml");
	buggy->TranslateLocal(float3(-0.15f, 0.0f, 0.20f));
	buggy->AddBehavior(Behaviors::BHVR_LOOK_AT);
	buggy->RemoveBehavior(Behaviors::BHVR_TURN_TO);
	buggy->OnMessage( &MessageT<GameObject*>( MSG_SET_TARGET, playerPtr ) );

	// create another buggy to show off the turn-to behavior
	GameObject* guy = m_TestScene.SpawnObject("GameObjects/GO_Player.xml");
	guy->TranslateLocal(float3(-0.15f, 0.0f, 0.10f));
	guy->RemoveBehavior(Behaviors::BHVR_LOOK_AT);
	guy->AddBehavior(Behaviors::BHVR_TURN_TO);
	guy->OnMessage( &MessageT<GameObject*>( MSG_SET_TARGET, playerPtr ) );

	// Create camera entity for first viewport
	cameras.push_back(m_TestScene.SpawnObject("GameObjects/GO_Camera1.xml"));
	cameras.back()->OnMessage( &MessageT<float3 *>(MSG_SET_OFFSET, &float3(0, 2, -5)));
	cameras.back()->OnMessage( &MessageT<UINT>(MSG_SET_PORT_INDEX, 0));

	cameras.back()->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/CameraKeyboardMapping.xml")); 
	cameras.back()->OnMessage( &MessageT<string>( MSG_INIT_MOUSE, "KeyMappings/CameraMouseMapping.xml")); 

	GameObject* lightSpherePtr = m_TestScene.SpawnObject( "GameObjects/GO_LightSphere.xml" );
	lightSpherePtr->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/PointLightKeyboardMapping.xml"));
	
	//// Rotating box
	float4x4 boxMat;
	boxMat.makeIdentity();
	//boxMat.rotateGlobalX(D3DX_PI/2.0f);
	//boxMat.rotateGlobalZ(D3DX_PI/6.0f);
	GameObject* testBoxPtr = m_TestScene.SpawnObject("GameObjects/GO_TestBox.xml", &boxMat);
	testBoxPtr->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/PointLightKeyboardMapping.xml"));
	// More objects for shadow map testing
	//const unsigned int NumObjects = 100;
	//const int MaxX = 100;
	//const int MaxZ = 100;
	//for(unsigned int i = 0; i < NumObjects; ++i)
	//{
	//	boxMat.WAxis.x = (float)(rand() % MaxX - MaxX /2);
	//	boxMat.WAxis.z = (float)(rand() % MaxZ - MaxZ /2);
	//	m_TestScene.SpawnObject("GameObjects/GO_TestBox.xml", &boxMat);
	//}

	// Spawn a bunch of transparent spheres to test transparency sorting
	float4x4 spehereMat;
	spehereMat.makeIdentity();
	float3 startLoc(10, 5, 0);
	spehereMat.WAxis = startLoc;
	float step = 2.0f;
	const float NumWide = 5;
	const float NumTall = 5;
	const float NumDeep = 5;

	for(float x = startLoc.x; x < NumWide * step + startLoc.x; x += step)
	{
		for(float y = startLoc.y; y < NumTall * step + startLoc.y; y += step)
		{
			for(float z = startLoc.z; z < NumDeep * step + startLoc.z; z += step)
			{
				spehereMat.WAxis = float3(x, y, z);
				m_TestScene.SpawnObject("GameObjects/GO_TransparentSphere.xml", &spehereMat);
			}
		}
	}

	// //My Test objects
	//float4x4 multiBoxMat;
	//multiBoxMat.makeIdentity();

	//GameObject* moveBox = m_TestScene.SpawnObject( "GameObjects/GO_TEST.xml", &multiBoxMat );
	//moveBox->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/PointLightKeyboardMapping.xml"));
	//moveBox->RemoveBehavior(Behaviors::BHVR_SPIN);
	//moveBox->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/PlayerKeyboardMapping.xml")); 
	//moveBox->OnMessage( &MessageT<string>( MSG_INIT_JOYSTICK, "KeyMappings/PlayerJoystickMapping.xml")); 

	//GameObject* spinBox = m_TestScene.SpawnObject( "GameObjects/GO_TEST.xml", &multiBoxMat );
	//spinBox->RemoveBehavior(Behaviors::BHVR_CONTROLLER);
	//spinBox->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/PointLightKeyboardMapping.xml"));

}