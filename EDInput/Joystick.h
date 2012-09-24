#pragma once

#define DIRECTINPUT_VERSION  0x0800
#include <XInput.h>
#include <dinput.h>

#pragma push_macro("new")
#undef new
#include <d3dx9math.h>
#pragma pop_macro("new")


#include <vector>
using std::vector;

#include <map>
using std::map;

namespace EDInput
{

enum JOYSTICK_TYPE { JOYSTICK_TYPE_XINPUT, JOYSTICK_TYPE_DIRECTINPUT, JOYSTICK_TYPE_UNINITIALIZED };

enum BUTTON_STATE { BUTTON_STATE_PRESSED  = 0x0001, 
				    BUTTON_STATE_HELD	  = 0x0002, 
				    BUTTON_STATE_RELEASED = 0x0004,
				    BUTTON_STATE_ANY	  = 0xFFFF};

   //////////////////////////////////////////////////////////////////////
  // Generic controller labels										  //
 // use these to assign game event triggers to joystick input events //
//////////////////////////////////////////////////////////////////////
enum JOYSTICK_EVENT {
					   ///////////
					  // D-Pad //
					 ///////////
					 DPAD_UP,
					 DPAD_DOWN,
					 DPAD_LEFT,
					 DPAD_RIGHT,
					   //////////////////
					  // face buttons //
					 //////////////////
					 BUTTON_0,
					 BUTTON_1,
					 BUTTON_2,
					 BUTTON_3,
					 BUTTON_START,
					 BUTTON_SELECT,
					   //////////////////////
					  // shoulder buttons //
					 //////////////////////
					 BUTTON_L1,
					 BUTTON_R1,
					 BUTTON_L2,
					 BUTTON_R2,
					   ////////////////////////
					  // thumbstick buttons //
					 ////////////////////////
					 BUTTON_LEFT_THUMB,
					 BUTTON_RIGHT_THUMB,
					   /////////////////
					  // thumbsticks //
					 /////////////////
					 LEFT_STICK_INPUT,
					 RIGHT_STICK_INPUT,
					   ///////////////////
					  // Xbox triggers //
					 ///////////////////
					 LEFT_TRIGGER_INPUT,
					 RIGHT_TRIGGER_INPUT,
					   ////////////////////////////////////////////
					  // Xbox alternates (used interchangeably) //
					 ////////////////////////////////////////////
					 BUTTON_A						=	BUTTON_1,
					 BUTTON_B						=	BUTTON_2,
					 BUTTON_X						=	BUTTON_0,
					 BUTTON_Y						=	BUTTON_3,
					 BUTTON_LEFT_BUMPER				=	BUTTON_L1,
					 BUTTON_RIGHT_BUMPER			=	BUTTON_R1 
				 };

struct ButtonEvent
{
	int button;
	int buttonState;
	ButtonEvent(int nButton, int nButtonState) : button(nButton), buttonState(nButtonState) {}
};

struct EDJoystickState
{
	unsigned			uiPlayerIndex;
	D3DXVECTOR2			vLeftStick;
	D3DXVECTOR2			vRightStick;
	float				fLeftTrigger;
	float				fRightTrigger;
	vector<ButtonEvent> ButtonEventList;	
};

////////////////////////
// Abstract Base Class 
//////////////////////
class EDJoystick
{
protected:
	JOYSTICK_TYPE				m_nJoystickType;	
	EDJoystickState				m_CurrentState;	
	map<int, JOYSTICK_EVENT>		m_InputMap;	

	virtual void LoadInputMap() = 0;

public:
	EDJoystick();
	virtual ~EDJoystick();
	virtual bool Initialize(unsigned uiPlayerIndex) = 0; 
	virtual void PollDeviceState() = 0;
	virtual void ParseDevice() = 0;
	
	JOYSTICK_TYPE	GetJoystickType()					{ return m_nJoystickType; }
	EDJoystickState &GetCurrentState()					{ return m_CurrentState; }
	unsigned		GetPlayerIndex()					{ return m_CurrentState.uiPlayerIndex; }	
	void			SetPlayerIndex(unsigned uiIndex)	{ m_CurrentState.uiPlayerIndex = uiIndex; }
};

////////////////////////
// XInput Device
//////////////////////
class EDJoystick_XInput : public EDJoystick
{
protected:
	XINPUT_STATE	m_XInputState;
	XINPUT_STATE	m_XInputPreviousState;

	XINPUT_STATE &GetXInputState() { return m_XInputState; }
	XINPUT_STATE &GetXInputPreviousState() { return m_XInputPreviousState; }
	// helper functions for ParseDevice()
	void ParseXInputButtons();
	void ParseXInputThumbsticks();
	void ParseXInputTriggers();
	// conversion map from XInput to generic
	void LoadInputMap();

public:
	EDJoystick_XInput(); 
	~EDJoystick_XInput();
	bool Initialize(unsigned uiPlayerIndex);
	void PollDeviceState();
	void ParseDevice();
};

////////////////////////
// DirectInput Device
//////////////////////
class EDJoystick_DirectInput : public EDJoystick
{
protected:
	IDirectInputDevice8*	m_DirectInputDevice;
	DIDEVCAPS				m_DirectInputDeviceCaps;
	DIJOYSTATE				m_DirectInputState;
	DIJOYSTATE				m_DirectInputPreviousState;

	DIDEVCAPS &GetDevicesCaps()					 { return m_DirectInputDeviceCaps; }
	DIJOYSTATE &GetDirectInputState()			 { return m_DirectInputState; }
	DIJOYSTATE &GetDirectInputPreviousState()	 { return m_DirectInputPreviousState; }
	// helper functions for ParseDevice()
	void ParseDirectInputButtons();
	void ParseDirectInputThumbsticks();
	void ParseDirectInputDPad();
	bool DPadIsPressed(int nDPadValue, JOYSTICK_EVENT DPadDirection);
	// conversion map from DirectInput to generic
	void LoadInputMap();

public:
	EDJoystick_DirectInput(); 
	~EDJoystick_DirectInput();
	bool Initialize(unsigned uiPlayerIndex);
	void PollDeviceState();
	void ParseDevice();		

	IDirectInputDevice8** GetDirectInputDevice() { return &m_DirectInputDevice; }	
};

} // end namespace EDInput