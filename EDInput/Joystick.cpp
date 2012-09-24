#include "../EDMemoryManager/MMAllocator.h"
#include "Joystick.h"

extern HWND		 globalHWnd;

namespace EDInput
{

  ///////////////////////////////////
 // Abstract Base Class functions //
///////////////////////////////////
EDJoystick::EDJoystick()
{
	m_nJoystickType = JOYSTICK_TYPE_UNINITIALIZED;
	m_CurrentState.uiPlayerIndex	= 0;
	m_CurrentState.vLeftStick.x		= 0.0f;
	m_CurrentState.vLeftStick.y		= 0.0f;
	m_CurrentState.vRightStick.x	= 0.0f;
	m_CurrentState.vRightStick.y	= 0.0f;
	m_CurrentState.fLeftTrigger		= 0.0f;
	m_CurrentState.fRightTrigger	= 0.0f;
}

EDJoystick::~EDJoystick()
{
}

  /////////////////////////////
 // XInput device functions //
/////////////////////////////
EDJoystick_XInput::EDJoystick_XInput()
{
	m_nJoystickType = JOYSTICK_TYPE_XINPUT; 

	ZeroMemory(&m_XInputState, sizeof(XINPUT_STATE));
	ZeroMemory(&m_XInputPreviousState, sizeof(XINPUT_STATE));

	LoadInputMap();
}

EDJoystick_XInput::~EDJoystick_XInput()
{
}

bool EDJoystick_XInput::Initialize(unsigned uiPlayerIndex)
{
	m_CurrentState.uiPlayerIndex = uiPlayerIndex;
	DWORD dwResult = XInputGetState(uiPlayerIndex, &GetXInputState());
	if (dwResult == ERROR_SUCCESS)
		return true;
	return false;
}

void EDJoystick_XInput::PollDeviceState()
{
	m_XInputPreviousState = m_XInputState;
	DWORD dwResult = XInputGetState(m_CurrentState.uiPlayerIndex, &m_XInputState);
}

void EDJoystick_XInput::ParseDevice()
{
	m_CurrentState.ButtonEventList.clear();
	// check for button presses and releases:
	ParseXInputButtons();
	ParseXInputThumbsticks();
	ParseXInputTriggers();
}

void EDJoystick_XInput::ParseXInputButtons()
{
	WORD &CurrentButtons =  GetXInputState().Gamepad.wButtons;
	WORD &PreviousButtons = GetXInputPreviousState().Gamepad.wButtons;

	for (int button = XINPUT_GAMEPAD_DPAD_UP; button <= XINPUT_GAMEPAD_Y; button <<= 1)
	{
		// determine the state of this button:
		int nState = 0;
		if (button & CurrentButtons)
		{				
			if (!(button & PreviousButtons))
				nState = BUTTON_STATE_PRESSED;
			else
				nState = BUTTON_STATE_HELD;
		}
		else if (button & PreviousButtons)
			nState = BUTTON_STATE_RELEASED;

		if (!nState)
			continue;
		// convert this API-specific input label to a genericly defined one and add it to the list of button events for this frame:
		int nJoystickEvent = m_InputMap[button];
		m_CurrentState.ButtonEventList.push_back(ButtonEvent(nJoystickEvent, nState));
	}
}

void EDJoystick_XInput::ParseXInputThumbsticks()
{
	// normalize the sticks
	float fShortMax = (float)SHRT_MAX;

	m_CurrentState.vLeftStick.x = (float)GetXInputState().Gamepad.sThumbLX / fShortMax;
	m_CurrentState.vLeftStick.y = (float)GetXInputState().Gamepad.sThumbLY / fShortMax;

	m_CurrentState.vRightStick.x = (float)GetXInputState().Gamepad.sThumbRX / fShortMax;
	m_CurrentState.vRightStick.y = (float)GetXInputState().Gamepad.sThumbRY / fShortMax;
}

void EDJoystick_XInput::ParseXInputTriggers()
{
	float fByteMax = (float)UCHAR_MAX;
	
	m_CurrentState.fLeftTrigger = (float)GetXInputState().Gamepad.bLeftTrigger / fByteMax;
	m_CurrentState.fRightTrigger = (float)GetXInputState().Gamepad.bRightTrigger / fByteMax;
}

void EDJoystick_XInput::LoadInputMap()
{
	m_InputMap[	XINPUT_GAMEPAD_A			 ]	=	BUTTON_A;
	m_InputMap[	XINPUT_GAMEPAD_B			 ]	=	BUTTON_B;
	m_InputMap[	XINPUT_GAMEPAD_X			 ]	=	BUTTON_X;
	m_InputMap[	XINPUT_GAMEPAD_Y			 ]	=	BUTTON_Y;
	m_InputMap[	XINPUT_GAMEPAD_START		 ]	=	BUTTON_START;
	m_InputMap[	XINPUT_GAMEPAD_BACK			 ]	=	BUTTON_SELECT;
	m_InputMap[	XINPUT_GAMEPAD_LEFT_SHOULDER ]	=	BUTTON_LEFT_BUMPER;
	m_InputMap[	XINPUT_GAMEPAD_RIGHT_SHOULDER]	=	BUTTON_RIGHT_BUMPER;
	m_InputMap[	XINPUT_GAMEPAD_LEFT_THUMB	 ]	=	BUTTON_LEFT_THUMB;
	m_InputMap[	XINPUT_GAMEPAD_RIGHT_THUMB	 ]	=	BUTTON_RIGHT_THUMB;
	m_InputMap[	XINPUT_GAMEPAD_DPAD_UP		 ]	=	DPAD_UP;
	m_InputMap[	XINPUT_GAMEPAD_DPAD_DOWN	 ]	=	DPAD_DOWN;
	m_InputMap[	XINPUT_GAMEPAD_DPAD_LEFT	 ]	=	DPAD_LEFT;
	m_InputMap[	XINPUT_GAMEPAD_DPAD_RIGHT	 ]	=	DPAD_RIGHT;
}

  //////////////////////////////////
 // DirectInput device functions //
//////////////////////////////////
EDJoystick_DirectInput::EDJoystick_DirectInput()
{
	m_nJoystickType = JOYSTICK_TYPE_DIRECTINPUT;

	m_DirectInputDevice = NULL;
	ZeroMemory(&m_DirectInputDeviceCaps, sizeof(DIDEVCAPS));
	ZeroMemory(&m_DirectInputState, sizeof(DIJOYSTATE));		
	ZeroMemory(&m_DirectInputPreviousState, sizeof(DIJOYSTATE));

	LoadInputMap();
}

EDJoystick_DirectInput::~EDJoystick_DirectInput()
{
	if (m_DirectInputDevice)
		m_DirectInputDevice->Release();
}

bool EDJoystick_DirectInput::Initialize(unsigned uiPlayerIndex)
{
	m_CurrentState.uiPlayerIndex = uiPlayerIndex;

	// get information about this device for use in parsing its data
	m_DirectInputDevice->Acquire();
	DIDEVICEINSTANCE device;
	device.dwSize = sizeof(device);
	m_DirectInputDevice->GetDeviceInfo(&device);
	m_DirectInputDeviceCaps.dwSize = sizeof(DIDEVCAPS);
	m_DirectInputDevice->GetCapabilities(&m_DirectInputDeviceCaps);

	// tell the device a little bit about ourselves and how we're going to use it
	m_DirectInputDevice->SetDataFormat(&c_dfDIJoystick);
	m_DirectInputDevice->SetCooperativeLevel(globalHWnd, /*DISCL_FOREGROUND |*/ DISCL_NONEXCLUSIVE);
	
	// set the range reported by the thumbsticks:
	DIPROPRANGE range;
	ZeroMemory				(&range, sizeof(DIPROPRANGE));
	range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	range.diph.dwSize		= sizeof(DIPROPRANGE);
	range.diph.dwHow		= DIPH_DEVICE;
	range.diph.dwObj		= 0;
	range.lMin				= SHRT_MIN;
	range.lMax				= SHRT_MAX;
	m_DirectInputDevice->SetProperty(DIPROP_RANGE, &range.diph);
	
	// set the deadzone to zero (because we're applying our own)
	DIPROPDWORD DIpdw;
	ZeroMemory				(&DIpdw, sizeof(DIPROPDWORD));
	DIpdw.diph.dwSize       = sizeof(DIPROPDWORD);
	DIpdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	DIpdw.diph.dwHow        = DIPH_DEVICE;	// setting deadzones for the whole device
	DIpdw.diph.dwObj		= 0;
	DIpdw.dwData            = 0;			// the deadzone
	HRESULT result			= m_DirectInputDevice->SetProperty(DIPROP_DEADZONE, &DIpdw.diph);

	return true;
}

void EDJoystick_DirectInput::PollDeviceState()
{
	m_DirectInputPreviousState = m_DirectInputState;
	m_DirectInputDevice->Acquire();
	m_DirectInputDevice->Poll();	
	m_DirectInputDevice->GetDeviceState(sizeof(DIJOYSTATE), &m_DirectInputState);
}

void EDJoystick_DirectInput::ParseDevice()
{
	m_CurrentState.ButtonEventList.clear();

	ParseDirectInputDPad();
	ParseDirectInputButtons();
	ParseDirectInputThumbsticks();
}

void EDJoystick_DirectInput::ParseDirectInputButtons()
{
	unsigned uiButtonCount = GetDevicesCaps().dwButtons;

	for (unsigned i = 0; i < uiButtonCount; i++)
	{
		// determine the state of this button:
		int nState = 0;
		if (GetDirectInputState().rgbButtons[i])
		{				
			if (!GetDirectInputPreviousState().rgbButtons[i])
				nState = BUTTON_STATE_PRESSED;
			else
				nState = BUTTON_STATE_HELD;
		}
		else if (GetDirectInputPreviousState().rgbButtons[i])
			nState = BUTTON_STATE_RELEASED;

		if (!nState)
			continue;
		// convert this API-specific input label to a genericly defined one and add it to the list of button events for this frame:
		int nJoystickEvent = m_InputMap[i];
		m_CurrentState.ButtonEventList.push_back(ButtonEvent(nJoystickEvent, nState));
	}
}

void EDJoystick_DirectInput::ParseDirectInputThumbsticks()
{
	// normalize the sticks
	float fShortMax = (float)SHRT_MAX;

	m_CurrentState.vLeftStick.x = (float)GetDirectInputState().lX / fShortMax;
	m_CurrentState.vLeftStick.y = -(float)GetDirectInputState().lY / fShortMax;

	m_CurrentState.vRightStick.x = (float)GetDirectInputState().lZ / fShortMax;
	m_CurrentState.vRightStick.y = -(float)GetDirectInputState().lRz / fShortMax;
}

void EDJoystick_DirectInput::ParseDirectInputDPad()
{
	// two whole functions just to parse this DPad value
	// get the d-pad state:
	int nDPad = GetDirectInputState().rgdwPOV[0];
	int nDPadPrevious = GetDirectInputPreviousState().rgdwPOV[0];
	
	if (nDPad == INT_MAX && nDPadPrevious == INT_MAX)  // for some reason "off" is equal to INT_MAX (?)
		return;
		
	for (int nDirection = DPAD_UP; nDirection <= DPAD_RIGHT; ++nDirection)
	{
		int nState = 0;
		JOYSTICK_EVENT DPadDirection = (JOYSTICK_EVENT)nDirection;
		if (DPadIsPressed(nDPad, DPadDirection))
		{
			if (!DPadIsPressed(nDPadPrevious, DPadDirection))
				nState = BUTTON_STATE_PRESSED;
			else
				nState = BUTTON_STATE_HELD;
		}
		else if (DPadIsPressed(nDPadPrevious, DPadDirection))
			nState = BUTTON_STATE_RELEASED;

		if (nState)
			m_CurrentState.ButtonEventList.push_back(ButtonEvent(nDirection, nState));
	}
}

bool EDJoystick_DirectInput::DPadIsPressed(int nDPadValue, JOYSTICK_EVENT DPadDirection)
{
	switch(DPadDirection)
	{
	case DPAD_UP:
		if (nDPadValue == 31500 || nDPadValue == 0 || nDPadValue == 4500)
			return true;
		return false;

	case DPAD_RIGHT:
		if (nDPadValue == 4500 || nDPadValue == 9000 || nDPadValue == 13500)
			return true;
		return false;

	case DPAD_DOWN:
		if (nDPadValue == 13500 || nDPadValue == 18000 || nDPadValue == 22500)
			return true;
		return false;

	case DPAD_LEFT:
		if (nDPadValue == 22500 || nDPadValue == 27000 || nDPadValue == 31500)
			return true;
		return false;
	}

	return false;
}

void EDJoystick_DirectInput::LoadInputMap()
{
	m_InputMap		[0]=		BUTTON_0 ;
	m_InputMap		[1]=		BUTTON_1 ;
	m_InputMap		[2]=		BUTTON_2 ;
	m_InputMap		[3]=		BUTTON_3 ;
	m_InputMap		[4]=		BUTTON_L1;
	m_InputMap		[5]=		BUTTON_R1;
	m_InputMap		[6]=		BUTTON_L2;
	m_InputMap		[7]=		BUTTON_R2;
	m_InputMap		[8]=		BUTTON_SELECT;
	m_InputMap		[9]=		BUTTON_START;
	m_InputMap		[10]=		BUTTON_LEFT_THUMB; 
	m_InputMap		[11]=		BUTTON_RIGHT_THUMB;
}

} // end namespace EDInput