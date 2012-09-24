#include "ArchGame.h"
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

/*Navigation System DLL*/
#include "../AINavigationManager/NavExports.h"

using namespace EDCollision;
using namespace EDInput;//::InputManager;
using namespace EDRendererD3D;
using namespace EDUtilities;

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
	
	DeclareMessage( MSG_SET_DIRECTIONAL_LIGHT );
	DeclareMessage( MSG_SET_DIRECTIONAL_LIGHT_CONTEXT );

	DeclareMessage( MSG_SET_LIGHT );
	DeclareMessage( MSG_SET_LIGHT_CONTEXT );
	DeclareMessage( MSG_SET_LOCAL );
	DeclareMessage( MSG_SET_TARGET );
}

ArchGame::ArchGame(void) 
{
	/*Navigation System 33*/
	InitNavigation();//Loads the DLL

	//m_pCamKeyboardController = 0;
	//m_pCameraEntity = 0;
	isPaused = false;
}

ArchGame::~ArchGame(void)
{
	/*Navigation System DLL*/
	/*Right now I don't have a game shutdown or level exit to clear the nav system*/
	releaseNavSystem(myNavSystem);//deletes the NavSystem
	releaseDebugRender(myNavDebugRender);//deletes the debug renderer
	ShutdownNavigation();//Unloads the DLL

	//m_Level.Clear();
}

void TogglePauseCallBackArch(unsigned int eventId, void *eventData, void *listener)
{
	((ArchGame *)globalGamePtr)->TogglePause();
}

void ExitCallBackArch(unsigned int eventId, void *eventData, void *listener)
{
	PostQuitMessage(0);
}

void ToggleActiveControllerCallBackArch(unsigned int eventId, void *eventData, void *listener)
{
    ((ArchGame *)globalGamePtr)->ToggleActiveController();
}

void ToggleActiveViewerCallBackArch(unsigned int eventId, void *eventData, void *listener)
{
	((ArchGame *)globalGamePtr)->ToggleActiveViewer();
}
void RenderSplashArch(unsigned int eventId, void *eventData, void *listener)
{
	((ArchGame*)globalGamePtr)->ExitSplashScreen();
}
void ArchGame::ExitSplashScreen()
{
	/*if(m_pSplashScreen)
	{
		m_pSplashScreen->RemoveComponent(m_pSplashScreen->GetComponent("RenderableComponent"));
		m_Level.GetEntityList().remove(m_pSplashScreen);
		delete m_pSplashScreen;
		m_pSplashScreen = 0;
		m_pPlayerController->SetEnabled(true);
	}*/
}
void ArchGame::ToggleActiveViewer()
{
	/*if( m_pActiveViewer == m_pCameraEntity )
		m_pActiveViewer = m_pRobotKid;
	else
		m_pActiveViewer = m_pCameraEntity;*/
}

void ArchGame::ToggleActiveController()
{
   // m_pSphereController->SetEnabled(!m_pSphereController->IsEnabled());
   // m_pPlayerController->SetEnabled(!m_pSphereController->IsEnabled());
}

void ArchGame::TogglePause()
{
	isPaused = !isPaused;
	if(isPaused)
		pauseEffect.Activate();
	else
		pauseEffect.Deactivate();
}

void ArchGame::Initialize() 
{
	ContentManager::theContentPath = "Resources/GDAssets/";
    
	GameBase::Initialize();

    InitTestObjects();
	
	EDEventManager::EventManager::GetInstance()->RegisterForEvent("Splash",0,RenderSplashArch);
	EDEventManager::EventManager::GetInstance()->RegisterForEvent("Exit", 0, ExitCallBackArch);
	InputManager::GetReference().AddKeyTrigger(VK_ESCAPE, INPUT_PRESSED, "Exit", EDInput::RAW_INPUT | EDInput::TYPED_INPUT);

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"PauseGame", 0, TogglePauseCallBackArch);
	InputManager::GetReference().AddKeyTrigger('P', INPUT_PRESSED, "PauseGame");

    EDEventManager::EventManager::GetInstance()->RegisterForEvent(
        "ToggleActiveController", 0, ToggleActiveControllerCallBackArch);
    InputManager::GetReference().AddKeyTrigger('Z', INPUT_PRESSED, "ToggleActiveController");

	InputManager::GetReference().SetTypingCallback(InputConsole::TypingCallback);

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"ToggleActiveViewer", 0, ToggleActiveViewerCallBackArch);
	InputManager::GetReference().AddKeyTrigger('V', INPUT_PRESSED, "ToggleActiveViewer");

	InputManager::GetReference().AddKeyTrigger('Q',INPUT_PRESSED,"ToggleControl");
    InputManager::GetReference().AddKeyTrigger('W', INPUT_HELD, "MovePlayerForward");
    InputManager::GetReference().AddKeyTrigger('S', INPUT_HELD, "MovePlayerBackward");
    InputManager::GetReference().AddKeyTrigger('A', INPUT_HELD, "MovePlayerLeft");
    InputManager::GetReference().AddKeyTrigger('D', INPUT_HELD, "MovePlayerRight");
    InputManager::GetReference().AddKeyTrigger('E', INPUT_HELD, "MoveUp");
    InputManager::GetReference().AddKeyTrigger('X', INPUT_HELD, "MoveDown");
    InputManager::GetReference().AddKeyTrigger('T', INPUT_HELD, "RotateRight");
    InputManager::GetReference().AddKeyTrigger('R', INPUT_HELD, "RotateLeft");
    InputManager::GetReference().AddKeyTrigger('G', INPUT_HELD, "RotateUp");
    InputManager::GetReference().AddKeyTrigger('F', INPUT_HELD, "RotateDown");
	InputManager::GetReference().AddKeyTrigger(VK_RETURN,INPUT_PRESSED,"Splash");

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

	//m_pActiveViewer = m_pCameraEntity;

	// Test pause sprite
	pauseSprite.LoadFromFile("GDModel/mdl_PauseScreen.xml", (float)WorldData::GetScreenWidth()/2,
		(float)WorldData::GetScreenHeight()/4, 100, 100);

	// print console background initialization
	consoleBackground.LoadFromFile("GDModel/mdl_ConsoleBackground.xml", (float)WorldData::GetScreenWidth()/2,
		(float)WorldData::GetScreenHeight()/8 * 3, 
		(float)WorldData::GetScreenWidth(), (float)WorldData::GetScreenHeight()/4 * 3, 
		0, 0.25f);

	pauseEffect.Initialize();

	/*Navigation System DLL*/
	myNavSystem = (AINavigation*)createNavSystem();//Creates the Navigation System
	NavMesh* mesh = myNavSystem->LoadNavMesh("Resources/navmesh.nsh");//Loads a navigation mesh
	tempAStar = myNavSystem->AddModule(AINavigation::ASTAR,mesh);//adds a navigation module of a given type & associates the mesh it searches
	myNavDebugRender = (NavDebugRender*)createDebugRender(myNavSystem,Renderer::theDevicePtr);//Creates & inits the debug render system associated with path finding
}

void ArchGame::Resize(int width, int height)
{
	// Update the camers with the new aspect ratio
	float projectionProperties[4] = { (float)width, (float)height, 0.1f, 200.0f };
	m_Frustum.Build( 0.78539816f, (float)width, (float)height, 0.1f, 200.0f);
	size_t numCameras = cameras.size();
	for(size_t i = 0; i < numCameras; ++i)
	{
		cameras[i]->OnMessage(&MessageT<float *>(MSG_BUILD_PROJECTION, projectionProperties));
	}
	GameBase::Resize(width, height);
}

void ArchGame::Update()
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
}

void ArchGame::Render()
{
    // clear the back buffer to blue
	Renderer::Clear(0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 0, 255), 1, 0); 
	Renderer::BeginScene();

	RenderController::GetInstance()->Render();

	/*Navigation System DLL*/
	myNavDebugRender->RenderNavMesh(tempAStar,ViewPortManager::GetReference().GetActiveProjection(),ViewPortManager::GetReference().GetActiveView());//Renders the mesh of a given module

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

void ArchGame::InitTestObjects()
{
	m_TestScene.SpawnObject("GameObjects/GO_Terrain.xml");
	m_TestScene.SpawnObject("GameObjects/GO_SkyBox.xml");

	float4x4 playerMat;
	playerMat.makeIdentity();
	playerMat.WAxis = float3( 0.0f, 0.0f, 2.0f );
	GameObject *playerPtr = m_TestScene.SpawnObject("GameObjects/GO_Player.xml", &playerMat);

	playerPtr->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/PlayerKeyboardMapping.xml")); 

	GameObject* buggy = m_TestScene.SpawnObject("GameObjects/GO_Buggy.xml");
	buggy->OnMessage( &MessageT<GameObject*>( MSG_SET_TARGET, playerPtr ) );		

	// Create camera entity for first viewport
	cameras.push_back(m_TestScene.SpawnObject("GameObjects/GO_Camera1.xml"));
	cameras.back()->OnMessage( &MessageT<float3 *>(MSG_SET_OFFSET, &float3(0, 2, -5)));
	cameras.back()->OnMessage( &MessageT<UINT>(MSG_SET_PORT_INDEX, 0));

	cameras.back()->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/CameraKeyboardMapping.xml")); 
	cameras.back()->OnMessage( &MessageT<string>( MSG_INIT_MOUSE, "KeyMappings/CameraMouseMapping.xml")); 

	GameObject* lightSpherePtr = m_TestScene.SpawnObject( "GameObjects/GO_LightSphere.xml" );
	lightSpherePtr->OnMessage( &MessageT<string>( MSG_INIT_KEYS, "KeyMappings/PointLightKeyboardMapping.xml"));
	
	//// Rotating box -  not rotating yet
	float4x4 boxMat;
	boxMat.makeIdentity();
	boxMat.rotateGlobalX(D3DX_PI/2.0f);
	boxMat.rotateGlobalZ(D3DX_PI/6.0f);

	m_TestScene.SpawnObject("GameObjects/GO_TestBox.xml", &boxMat);
}
