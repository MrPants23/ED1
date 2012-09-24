#pragma once

#pragma push_macro("new")
#undef new
#include <d3dx9math.h>
#pragma pop_macro("new")


#include "InputManager.h"
#include "Joystick.h"

using std::vector;

namespace EDInput
{

#define MAX_JOYSTICKS 4
#define NORMALIZED_DEADZONE 0.16f
#define NORMALIZED_TRIGGER_THRESHOLD 0.02f		// deadzones should probably be switched to variables	

	BOOL CALLBACK DirectInputCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext);

	// used to attach PlayerIndex and analog position data (if necessary) to events posted by the JoystickManager
	struct PlayerInputReport
	{
		unsigned uiPlayerIndex;
		void	 *dataPtr;
	};

	// only used internally by JoystickManager:
	struct JoystickEventMessage
	{
		string m_eventName;
		uint16_t    m_state;
		uint16_t    m_mode;

		JoystickEventMessage(const string &eventName, uint16_t state, uint16_t mode) : m_eventName(eventName), m_state(state), m_mode(mode) { }
		inline bool IsValidState(BUTTON_STATE state) const	{ return (m_state & state) != 0; }
		inline bool IsValidMode(InputMode mode) const		{ return (m_mode & mode)   != 0; }
	};

	class JoystickManager
	{
	public:

		// destructor/singleton functionality:
		~JoystickManager(void);
		static JoystickManager &GetReference();	
		//static void DeleteInstance();	

		// should be called when WinProc processes a WM_DEVICECHANGE message. 
		// updates the m_ConnectedJoysticks list
		void RefreshDevices();

		// only public for the sake of DirectInputCallback; could be fixed with friending?
		void AddJoystickDevice(EDJoystick *pNewJoystick);

		// must be called every frame to poll devices since XInput and DirectInput do not generate windows messages automatically
		// added as a callback to a persistent event posted when the JoystickManager is initialized
		static void Update(unsigned int eventId, void *eventData, void *listener);

		// adds an event to the list of game events mapped to a particular joystick input.
		// "JoystickEvent" is one of the generic JOYSTICK_EVENT enums at the top of Joystick.h
		// "state" is one or more of the BUTTON_STATEs enumed at the top of Joystick.h and will restrict the event to that button state or states accordingly
		// "mode" is the InputManager mode or modes within which we want to trigger this event
		// for usage example see definition of LoadFSGDGameTriggers() declared below
		void AddJoystickTrigger(uint16_t JoystickEvent, const string &eventName, int state = (BUTTON_STATE_PRESSED | BUTTON_STATE_HELD), int mode = MODE_ANY);

		// clears the list of currently mapped triggers
		void ClearJoystickTriggers();

		// loads all the button/function mapping for the main FSGDGame. Re-map joystick inputs here.	
		void LoadFSGDGameTriggers();

		// accessors:
		unsigned			   GetJoystickCount()							{ return m_uiJoystickCount; }
		IDirectInput8*		   GetDirectInputObject()						{ return m_DI; }

		// allows objects to directly read the generic version of the current state of any connected joysticks.
		// returns NULL if no joysticks are connected or if uiPlayerIndex is not a currently connected index.
		const EDJoystickState* GetJoystickState(unsigned uiPlayerIndex);

		uint16_t GetKeyByName(std::string keyName) 
		{ 
			return keyCodeMapping[keyName]; 
		}

		BUTTON_STATE GetKeyStateByName(std::string stateName);
	private:

		// singleton functionality
		JoystickManager(void);
		JoystickManager(const JoystickManager&) {};
		JoystickManager &operator=(const JoystickManager &) {}

		//static JoystickManager *instancePtr;	

		// the number and list of controllers connected:
		unsigned			m_uiJoystickCount;
		vector<EDJoystick*> m_ConnectedJoysticks;		

		// DirectInput object:
		IDirectInput8*			m_DI;

		// List of game events mapped to each possible input event:
		map<Trigger, list<JoystickEventMessage>> m_JoystickTriggers;	

		// Set of names matched to enum values for joystick keys
		map<std::string, uint16_t> keyCodeMapping;
		void BuildKeyCodeMapping(void);

		// Initializes DirectInput object and does an initial check for connected joystick or controller devices. 
		// Called automatically by constructor.
		void Initialize();

		// deletes EDJoystick objects and clears the list
		void ClearConnectedJoysticks();

		// stores the previous and populates the current state for any connected controllers
		void PollDevices();

		// Parses all connected controller data populated by PollDevices()
		void PostJoystickEvents();

		// searches the event map for a given JSEvent and actually posts to the EventManager if necessary:
		void PostEvent(JOYSTICK_EVENT JSEvent, void* dataPtr, BUTTON_STATE state = BUTTON_STATE_ANY );

		// Checks the passed-in thumbstick vector against a circular deadzone and modifies it accordingly.
		// Vector is re-mapped to the live range if it exceeds the deadzone or zeroed if it does not.
		// In/Out: 2D vector representing normalized (-1 to 1 range) x and y values for the thumbstick in question.
		// Return: True if the stick exceeds the deadzone, false if not.
		// Circular deadzone produces more accurate 360 degree movement than square deadzone.
		// use when both axes of the thumbstick represent similar type of movement or rotation on 2 axes (whole stick does movement, or looking, or pitch/roll, etc...).
		bool ApplyDeadzoneCircular(D3DXVECTOR2 &StickVec);

		// Checks the passed-in thumbstick vector against a square deadzone and modifies each axis accordingly.
		// Each axis is independently re-mapped to the live range if it exceeds the deadzone or zeroed if it does not.
		// In/Out: 2D vector representing normalized (-1 to 1 range) x and y values for the thumbstick in question.
		// Return: True if the stick exceeds the deadzone on either axis, false if not.
		// square deadzone clamps to 1 of the 4 primary directions when StickVec is nearly horizontal or vertical (but still has a magnitude).
		// use when each axis of the thumbstick is mapped to totally different functionality (ie steering and throttle on the same stick)
		bool ApplyDeadzoneSquare(D3DXVECTOR2 &StickVec);

		// checks the passed-in float against the normalized trigger threshold and re-maps it to the live range accordingly.
		// In/Out: normalized (0-1 range) trigger position
		// Return: True if the trigger has exceeded the threshold, false if not.
		bool ApplyTriggerDeadzone(float &fTrigger);
	};

}  // end namespace EDInput