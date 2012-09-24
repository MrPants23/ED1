#include "EDGOCAGame.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
#include "../EDRendererD3D/Renderer.h"
#include "../EDRendererD3D/ViewPortManager.h"

#include "../EDUtilities/DebugRenderer.h"
#include "../EDUtilities/PrintConsole.h"

#include "InputConsole.h"

#include "RenderController.h"

#include "Behaviors.h"

using namespace EDCollision;
using namespace EDInput;//::InputManager;
using namespace EDRendererD3D;
using namespace EDUtilities;

extern HWND globalHWnd;
extern GameBase *globalGamePtr;

#include "../EDGOCA/ComponentSystem.h"
#include "../EDGOCA/MessageT.h"
using namespace EDGOCA;

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
}

EDGOCAGame::EDGOCAGame(void) 
{
}

EDGOCAGame::~EDGOCAGame(void)
{
}

void ExitCallBackGOCA(unsigned int eventId, void *eventData, void *listener)
{
	PostQuitMessage(0);
}

void EDGOCAGame::Initialize() 
{
	ContentManager::theContentPath = "Resources/GDAssets/";
    
	GameBase::Initialize();

    InitTestObjects();
	
	EDEventManager::EventManager::GetInstance()->RegisterForEvent("Exit", 0, ExitCallBackGOCA);
	InputManager::GetReference().AddKeyTrigger(VK_ESCAPE, INPUT_PRESSED, "Exit", EDInput::RAW_INPUT | EDInput::TYPED_INPUT);

	InputManager::GetReference().SetTypingCallback(InputConsole::TypingCallback);

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
}

void EDGOCAGame::Resize(int width, int height)
{
	GameBase::Resize(width, height);

	float projectionProperties[4] = { (float)width, (float)height, 0.1f, 100.0f };
	size_t numCameras = cameras.size();
	for(size_t i = 0; i < numCameras; ++i)
	{
		cameras[i]->OnMessage(&MessageT<float *>(MSG_BUILD_PROJECTION, projectionProperties));
	}
}

void EDGOCAGame::Update()
{
	GameBase::Update();

	if( GetAsyncKeyState( 'I' ) )
		m_TestObject->OnMessage( &IMessage(MSG_MOVE_FORWARD) );
		//m_TestObject->OnMessage( &MessageT<float3>( MSG_APPLY_FORCE, m_TestObject->GetWorldTransform().ZAxis * 20.0f ) );
	if( GetAsyncKeyState( 'K' ) )
		m_TestObject->OnMessage( &IMessage(MSG_MOVE_BACKWARD) );
	if( GetAsyncKeyState( 'U' ) )
		m_TestObject->OnMessage( &IMessage(MSG_MOVE_LEFT) );
	if( GetAsyncKeyState( 'O' ) )
		m_TestObject->OnMessage( &IMessage(MSG_MOVE_RIGHT) );
	if( GetAsyncKeyState( 'J' ) )
		m_TestObject->OnMessage( &IMessage(MSG_TURN_LEFT) );
	if( GetAsyncKeyState( 'L' ) )
		m_TestObject->OnMessage( &IMessage(MSG_TURN_RIGHT) );
	if( GetAsyncKeyState( VK_SPACE ) )
		m_TestObject->OnMessage( &IMessage(MSG_MOVE_UP) );
		//m_TestObject->OnMessage( &MessageT<float3>( MSG_APPLY_FORCE, float3(0.0f, 1000.0f, 0.0f) ) );

	m_TestScene.Update();

	// Update frustum
// TODO - do this per viewport
	FrustumHull tempFrustum = m_Frustum;
	tempFrustum.Transform( ViewPortManager::GetReference().GetActiveWorld() );
	m_TestScene.FrustumCull( &tempFrustum, &MessageT<FrustumHull*>( MSG_FRUSTUM_TEST, &tempFrustum ) );

    RenderController::GetInstance()->BuildDirViewProjection(tempFrustum);

	//Sphere temp;
	//temp.m_Center = m_TestObject->GetWorldTransform().WAxis;
	//temp.m_Radius = 0.5f;

	//DebugRenderer::GetInstance()->DrawSphere( temp );

	DebugRenderer::GetInstance()->RenderDebugPrimitives();
}

void EDGOCAGame::Render()
{
    // clear the back buffer to blue
	Renderer::Clear(0,0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1, 0); 
	Renderer::BeginScene();
	
	RenderController::GetInstance()->Render();
	
    PrintConsole::GetReference().Render();
	InputConsole::GetReference().Render();
	
	Renderer::EndScene();
	
	// buffer swap
	Renderer::Present(0,0,0,0);
}

void EDGOCAGame::InitTestObjects()
{
	m_TestObject = m_TestScene.SpawnObject("adkjaldj");
	//m_TestObject->AddBehavior( Behaviors::BHVR_CONTROLLER );
	m_TestObject->TranslateLocal( float3(0.0f, 0.0f, 2.0f) );

	m_TestObject->AddBehavior( Behaviors::BHVR_CHARACTER_CONTROLLER );
	m_TestObject->AddBehavior( Behaviors::BHVR_COLLIDER );
	m_TestObject->AddBehavior( Behaviors::BHVR_RENDERABLE );
	m_TestObject->OnMessage( &MessageT<string>( MSG_SET_RENDERSHAPE, 
		"GDForm/VZ_Robot_Walk/robotkid/robotkidShape/frm_mat_blinn1_rc_LitTexturedShadows.xml" ) );

	//m_TestObject->AddBehavior( Behaviors::BHVR_PHYSICS );
	//m_TestObject->AddBehavior( Behaviors::BHVR_GROUNDCLAMP );

	GameObject* pGroundObj = m_TestScene.SpawnObject("static");
	pGroundObj->AddBehavior( Behaviors::BHVR_RENDERABLE );
	pGroundObj->OnMessage( &MessageT<string>( MSG_SET_RENDERSHAPE, 
		"GDForm/terrain/pPlane1/pPlaneShape1/frm_mat_blinn1_rc_LitTextured.xml" ) );
	pGroundObj->AddBehavior( Behaviors::BHVR_COLLIDER );
	pGroundObj->OnMessage( &MessageT<string>( MSG_SET_COLLIDER_MESH, 
		"GDMesh/terrain/pPlaneShape1.bin" ) );

	GameObject* bob = m_TestScene.SpawnObject("bob");
	//m_TestObject->AddChild( bob );

	bob->AddBehavior( Behaviors::BHVR_CHARACTER_CONTROLLER );
	bob->AddBehavior( Behaviors::BHVR_COLLIDER );
	bob->AddBehavior( Behaviors::BHVR_RENDERABLE );
	bob->OnMessage( &MessageT<string>( MSG_SET_RENDERSHAPE, "GDForm/VZ_Robot_Walk/robotkid/robotkidShape/frm_mat_blinn1_rc_LitTexturedShadows.xml" ) );
	//bob->OnMessage( &MessageT<string>( MSG_SET_RENDERSHAPE, 
		//"GDForm/GDBuggy/Body/BodyShape/frm_mat_lambert3_rc_LitTexturedShadows.xml"));

	// Create camera entity for first viewport
	cameras.push_back(m_TestScene.SpawnObject("Camera1"));
	cameras.back()->AddBehavior(Behaviors::BHVR_HARD_ATTACH);
	cameras.back()->OnMessage( &MessageT<GameObject *>(MSG_ATTACH, m_TestObject));
	cameras.back()->OnMessage( &MessageT<float3 *>(MSG_SET_OFFSET, &float3(0, 2, -5)));
	cameras.back()->AddBehavior(Behaviors::BHVR_CAMERA);
	cameras.back()->OnMessage( &MessageT<UINT>(MSG_SET_PORT_INDEX, 0));
}


