#include "../EDMemoryManager/MMAllocator.h"
#include "JoystickManager.h"

extern HINSTANCE hInst;

namespace EDInput
{

	//JoystickManager *JoystickManager::instancePtr = NULL;

	JoystickManager::JoystickManager(void) 
	{	
		m_uiJoystickCount = 0;
		m_DI			  = NULL;

		Initialize();
	}

	JoystickManager::~JoystickManager(void)
	{
		if (m_DI)
			m_DI->Release();
		ClearConnectedJoysticks();
		EDEventManager::EventManager::GetInstance()->UnRegisterForEvent("JoystickManagerUpdate", this);
	}

	JoystickManager &JoystickManager::GetReference()
	{	
		static JoystickManager instance;
		return instance;

		// had to change instantiation method because the singleton instantiation method commented out below was being defeated by the DirectInputCallback function.
		// a new copy of the JoystickManager was being created each time the callback tried to GetInstance().

		//if(0 == instancePtr)
		//{
		//	instancePtr = new JoystickManager();
		//}
		//return *instancePtr;
	}

	//void JoystickManager::DeleteInstance()
	//{
	//	if (instancePtr)
	//		delete instancePtr;
	//	instancePtr = NULL;
	//}

	const EDJoystickState* JoystickManager::GetJoystickState(unsigned uiPlayerIndex)
	{
		if (uiPlayerIndex >= m_uiJoystickCount)
			return NULL;

		return &m_ConnectedJoysticks[uiPlayerIndex]->GetCurrentState();
	}

	void JoystickManager::ClearConnectedJoysticks()
	{
		for (unsigned i = 0; i < m_uiJoystickCount; ++i)
			delete m_ConnectedJoysticks[i];
		m_ConnectedJoysticks.clear();
		m_uiJoystickCount = 0;
	}

	void JoystickManager::Initialize()
	{	
		// Initialize the DirectInput Object:
		HRESULT InitResult = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DI, 0);

		RefreshDevices();

		// register for and post the persistent event that will cause our Update function to be called every frame
		EDEventManager::EventManager *EM = EDEventManager::EventManager::GetInstance();
		EM->RegisterForEvent("JoystickManagerUpdate", (void*)this, Update);
		EM->PostEvent("JoystickManagerUpdate", 0, EDEventManager::EM_PRIORITY_HIGH, EDEventManager::EM_FLAG_PERSISTENT);

		BuildKeyCodeMapping();
	}

	void JoystickManager::RefreshDevices()
	{
		ClearConnectedJoysticks();

		// get the list of all connected controllers from DirectInput
		m_DI->EnumDevices(DI8DEVCLASS_GAMECTRL, &DirectInputCallback, 0, DIEDFL_ATTACHEDONLY);	
		// DirectInputCallback() has pushed an EDJoystick_DirectInput onto m_ConnectedJoysticks for every controller connected.
		// now loop through that list and check each for XInput compatibility, replacing with EDJoystick_XInput where appropriate
		for (unsigned i = 0; i < m_uiJoystickCount; i++)
		{
			EDJoystick_XInput *pXIJoystick = new EDJoystick_XInput;
			if (pXIJoystick->Initialize(i))
			{
				// delete the DI Joystick and replace it with the XInput one:
				delete m_ConnectedJoysticks[i];		   // destructor releases the associated DIDevice
				m_ConnectedJoysticks[i] = pXIJoystick; // XInput requires no additional initialization
			}
			// if it's not an Xbox controller complete the DirectInput initialization:
			else
			{
				delete pXIJoystick;
				m_ConnectedJoysticks[i]->Initialize(i);
			}
		}
	}

	BOOL CALLBACK DirectInputCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
	{
		JoystickManager &JSM = JoystickManager::GetReference();
		// try to create a new DirectInput device:
		EDJoystick_DirectInput *pDIJoystick = new EDJoystick_DirectInput;
		JSM.GetDirectInputObject()->CreateDevice( pdidInstance->guidInstance, pDIJoystick->GetDirectInputDevice(), NULL);
		// now see if the pointer was actually populated by the DirectInput object:
		if (pDIJoystick->GetDirectInputDevice())
		{
			JSM.AddJoystickDevice(pDIJoystick);
			return DIENUM_CONTINUE;
		}
		delete pDIJoystick;
		return DIENUM_STOP;	
	}

	void JoystickManager::AddJoystickDevice(EDJoystick *pNewJoystick)
	{
		m_ConnectedJoysticks.push_back(pNewJoystick);
		m_uiJoystickCount = static_cast<unsigned int>(m_ConnectedJoysticks.size());
		unsigned uiIndex = m_uiJoystickCount - 1;
		m_ConnectedJoysticks[uiIndex]->SetPlayerIndex(uiIndex);
	}

	void JoystickManager::Update(unsigned int eventId, void *eventData, void *listener)
	{
		JoystickManager *JM = (JoystickManager*)listener;
		JM->PollDevices();
		JM->PostJoystickEvents();
	}

	void JoystickManager::PollDevices()
	{
		for (unsigned i = 0; i < m_uiJoystickCount; ++i)
			m_ConnectedJoysticks[i]->PollDeviceState();
	}

	void JoystickManager::PostJoystickEvents()
	{
		for (unsigned i = 0; i < m_uiJoystickCount; ++i)
		{
			// tell the device to convert its API-specific deviceState to a generic one:
			m_ConnectedJoysticks[i]->ParseDevice();
			EDJoystickState &CurrentState	= m_ConnectedJoysticks[i]->GetCurrentState();
			unsigned int uiButtonEvents			= static_cast<unsigned int>(CurrentState.ButtonEventList.size());
			// create the report that will be attached to any generated events to convey playerIndex and any necessary controller position 
			PlayerInputReport InputReport;
			InputReport.uiPlayerIndex		= i;
			InputReport.dataPtr				= NULL;
			void* pReport					= (void*)&InputReport;

			// apply all deadzones before sending events (in case someone decides to access this joystick state in response to receiving an event)
			bool bLeftStick    = ApplyDeadzoneCircular(CurrentState.vLeftStick);
			bool bRightStick   = ApplyDeadzoneCircular(CurrentState.vRightStick);
			bool bLeftTrigger  = ApplyTriggerDeadzone(CurrentState.fLeftTrigger);
			bool bRightTrigger = ApplyTriggerDeadzone(CurrentState.fRightTrigger);

			// send button events
			for (unsigned j = 0; j < uiButtonEvents; ++j)
				PostEvent((JOYSTICK_EVENT)CurrentState.ButtonEventList[j].button, pReport, (BUTTON_STATE)CurrentState.ButtonEventList[j].buttonState);

			// send joystick events:
			if (bLeftStick)
			{
				InputReport.dataPtr = (void*)&CurrentState.vLeftStick;
				PostEvent(LEFT_STICK_INPUT, pReport);
			}
			if (bRightStick)
			{
				InputReport.dataPtr = (void*)&CurrentState.vRightStick;
				PostEvent(RIGHT_STICK_INPUT, pReport);
			}

			// send trigger events:
			if (bLeftTrigger)			
			{
				InputReport.dataPtr = (void*)&CurrentState.fLeftTrigger;
				PostEvent(LEFT_TRIGGER_INPUT, pReport);
			}
			if (bRightTrigger)			
			{
				InputReport.dataPtr = (void*)&CurrentState.fRightTrigger;
				PostEvent(RIGHT_TRIGGER_INPUT, pReport);
			}
		}	
	}

	void JoystickManager::PostEvent(JOYSTICK_EVENT JSEvent, void* dataPtr, BUTTON_STATE state)
	{
		// search for event messages that are mapped to this input event
		Trigger trigger(JSEvent);
		InputMode Mode = InputManager::GetReference().GetInputMode();
		for (list<JoystickEventMessage>::iterator message = m_JoystickTriggers[trigger].begin(); message != m_JoystickTriggers[trigger].end(); message++)
		{
			// Is this the right input mode and button state for this event? If so, activate this trigger.
			if (message->IsValidMode(Mode) && message->IsValidState(state))
				EDEventManager::EventManager::GetInstance()->PostEvent(message->m_eventName, dataPtr, EDEventManager::EM_PRIORITY_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
		}
	}

	bool JoystickManager::ApplyDeadzoneCircular(D3DXVECTOR2 &StickVec)
	{
		float fMagSquared = D3DXVec2Dot(&StickVec, &StickVec);
		float fDeadzoneSquared = NORMALIZED_DEADZONE * NORMALIZED_DEADZONE;
		if (fMagSquared < fDeadzoneSquared) 
		{
			StickVec *= 0.0f;
			return false;
		}
		// re-map the value to the live range to account for the deadzone:
		float fMag = sqrt(fMagSquared);
		D3DXVECTOR2 StickVecMax = StickVec / fMag; 
		D3DXVECTOR2 StickVecMin = StickVecMax * NORMALIZED_DEADZONE;
		StickVec -= StickVecMin;
		StickVec /= 1.0f - NORMALIZED_DEADZONE;
		return true;
	}

	bool JoystickManager::ApplyDeadzoneSquare(D3DXVECTOR2 &StickVec)
	{
		// square deadzone is axis-independent:
		if (abs(StickVec.x) < NORMALIZED_DEADZONE)
			StickVec.x = 0.0f;
		if (abs(StickVec.y) < NORMALIZED_DEADZONE)
			StickVec.y = 0.0f;
		if (StickVec.x == 0.0f && StickVec.y == 0.0f)
			return false;

		// re-map the value to the live range to account for the deadzone:
		float fLiveRange = 1.0f - NORMALIZED_DEADZONE;

		if (StickVec.x > 0.0f)
			StickVec.x = (StickVec.x - NORMALIZED_DEADZONE) / fLiveRange;
		else if (StickVec.x < 0.0f)
			StickVec.x = (StickVec.x + NORMALIZED_DEADZONE) / fLiveRange;
		if (StickVec.y > 0.0f)
			StickVec.y = (StickVec.y - NORMALIZED_DEADZONE) / fLiveRange;
		else if (StickVec.y < 0.0f)
			StickVec.y = (StickVec.y + NORMALIZED_DEADZONE) / fLiveRange;

		return true;
	}

	bool JoystickManager::ApplyTriggerDeadzone(float &fTrigger)
	{
		if (fTrigger < NORMALIZED_TRIGGER_THRESHOLD)
		{	
			fTrigger = 0.0f;
			return false;
		}
		fTrigger -= NORMALIZED_TRIGGER_THRESHOLD;
		fTrigger /= 1.0f - NORMALIZED_TRIGGER_THRESHOLD;
		return true;
	}

	void JoystickManager::AddJoystickTrigger(uint16_t JoystickEvent, const string &eventName, int state, int mode)
	{
		// Insert a trigger for this Joystick Input Event.
		m_JoystickTriggers[Trigger(JoystickEvent)].push_back(JoystickEventMessage(eventName, state, mode));
	}

	void JoystickManager::ClearJoystickTriggers()
	{
		m_JoystickTriggers.clear();
	}

	void JoystickManager::LoadFSGDGameTriggers()
	{
		return;
		// re-map joystick inputs here.
		// unless specified default button state is BUTTON_STATE_PRESSED or BUTTON_STATE_HELD.
		// mode can also be restricted as the 4th argument, default is to allow joystick input in either RAW_INPUT or TYPED_INPUT mode.
		// use the generic "JOYSTICK_EVENT" descriptors enumed at the top of Joystick.h (as done here).

		// see "CamMouseLookController.cpp" for examples of joystick input handling. When a joystick event message is broadcast a PlayerInputReport is attached to the event.
		// the report contains the player index that the joystick input originated from and any controller value associated with the input type (normalized thumbstick or trigger position)
		// input handling objects should recast the "eventData" null pointer to a PlayerInputReport pointer to access this information.
		// right now input from any joystick is being processed without regard to the Joystick index it originated from.

		ClearJoystickTriggers();

		// control mode switching
		AddJoystickTrigger( BUTTON_START,			"PauseGame"				,   BUTTON_STATE_PRESSED);  // restrict this event to button presses only
		AddJoystickTrigger(	BUTTON_SELECT,			"ToggleActiveController",	BUTTON_STATE_PRESSED);	
		// camera y-axis inversion toggling
		AddJoystickTrigger( BUTTON_RIGHT_THUMB,		"InvertYToggle"			,	BUTTON_STATE_PRESSED);
		// player/entity movement
		AddJoystickTrigger(	DPAD_UP,				"MoveForward"			);
		AddJoystickTrigger(	DPAD_DOWN,				"MoveBackward"			);
		AddJoystickTrigger(	DPAD_LEFT,				"MoveLeft"				);
		AddJoystickTrigger(	DPAD_RIGHT,				"MoveRight"				);
		AddJoystickTrigger( BUTTON_RIGHT_BUMPER,	"MoveUp"				);
		AddJoystickTrigger( BUTTON_LEFT_BUMPER,		"MoveDown"				);
		// player/entity rotation
		AddJoystickTrigger(	BUTTON_B,				"RotateRight"			);
		AddJoystickTrigger(	BUTTON_X,				"RotateLeft"			);
		AddJoystickTrigger(	BUTTON_Y,				"RotateUp"				);
		AddJoystickTrigger(	BUTTON_A,				"RotateDown"			);			
		// camera movement and rotation
		AddJoystickTrigger(	LEFT_STICK_INPUT,		"CameraJoystickMove"	);
		AddJoystickTrigger(	RIGHT_STICK_INPUT,		"CameraJoystickRotate"	);
		AddJoystickTrigger(	LEFT_TRIGGER_INPUT,		"CameraMoveDown"		);
		AddJoystickTrigger(	RIGHT_TRIGGER_INPUT,	"CameraMoveUp"			);	
		AddJoystickTrigger(	BUTTON_L2,				"CameraMoveDown"		);
		AddJoystickTrigger(	BUTTON_R2,				"CameraMoveUp"			);
	}

	void JoystickManager::BuildKeyCodeMapping(void)
	{
		keyCodeMapping.insert(pair<std::string, uint16_t>("DPAD_UP", DPAD_UP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("DPAD_DOWN", DPAD_DOWN));
		keyCodeMapping.insert(pair<std::string, uint16_t>("DPAD_LEFT", DPAD_LEFT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("DPAD_RIGHT", DPAD_RIGHT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_0", BUTTON_0));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_1", BUTTON_1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_2", BUTTON_2));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_3", BUTTON_3));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_START", BUTTON_START));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_SELECT", BUTTON_SELECT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_L1", BUTTON_L1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_R1", BUTTON_R1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_L2", BUTTON_L2));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_R2", BUTTON_R2));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_LEFT_THUMB", BUTTON_LEFT_THUMB));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_RIGHT_THUMB", BUTTON_RIGHT_THUMB));
		keyCodeMapping.insert(pair<std::string, uint16_t>("LEFT_STICK_INPUT", LEFT_STICK_INPUT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("LEFT_TRIGGER_INPUT", LEFT_TRIGGER_INPUT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("RIGHT_STICK_INPUT", RIGHT_STICK_INPUT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("RIGHT_TRIGGER_INPUT", RIGHT_TRIGGER_INPUT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_A", BUTTON_A));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_B", BUTTON_B));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_X", BUTTON_X));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_Y", BUTTON_Y));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_LEFT_BUMPER", BUTTON_LEFT_BUMPER));
		keyCodeMapping.insert(pair<std::string, uint16_t>("BUTTON_RIGHT_BUMPER", BUTTON_RIGHT_BUMPER));
	}

	BUTTON_STATE JoystickManager::GetKeyStateByName(std::string stateName)
	{
		BUTTON_STATE returnState;

		if("Released" == stateName)
		{
			returnState = BUTTON_STATE_RELEASED;
		}
		else if("Pressed" == stateName)
		{
			returnState = BUTTON_STATE_PRESSED;
		}
		else if("Any" == stateName)
		{
			returnState = BUTTON_STATE_ANY;
		}
		else if("Held" == stateName)
		{
			returnState = BUTTON_STATE_HELD;
		}

		return returnState;
	}
} // end namespace EDInput