#pragma once

#include <stdint.h>

#include "../EDEventManager/EventManager.h"
using EDEventManager::EventManager;

#include <utility>
#include <string>
#include <list>
#include <map>
using std::pair;

using std::make_pair;
using std::pair;
using std::string;
using std::list;
using std::map;

#include "MouseState.h"

namespace EDInput
{
	/// Enumeration of possible input states (8 bits)
	enum InputMode { RAW_INPUT = 0x0001, TYPED_INPUT = 0x0002, MODE_ANY = 0x00FF };	///< Enumeration of possible input mode flags (16 bits)

	enum ButtonState { INPUT_RELEASED, INPUT_PRESSED, INPUT_REPEATED, INPUT_HELD };	///< State of the key - released, pressed, repeated, or held (8 bits)

	enum KeyData { KEY_NONE = 0, KEY_LEFT, KEY_RIGHT };			///< Side of the keyboard this key is from (8 bits)

	enum MouseData { POSITION_CHANGED, CURSOR_MOVED, WHEEL_MOVED, H_WHEEL_MOVED }; ///< State of the mouse (8 bits)

	// Used only internally within the input manager (key to access events to trigger based on input)
	struct Trigger
	{
		uint16_t button;			///< Button data (what button / key does this apply to, if any)
		ButtonState buttonState;		///< State of button, if any - released, pressed, repeated, or held (8 bits)
		union
		{
			KeyData keyData;
			MouseData mouseData;
			uint16_t deviceData;
		};							///< Device specific information (side of a keyboard, mouse positions, etc)

		Trigger(uint16_t _button, ButtonState _buttonState, KeyData _keyData = KEY_NONE) :  button(_button), buttonState(_buttonState), keyData(_keyData) { }
		Trigger(MouseData _mouseData) : mouseData(_mouseData) { }
		Trigger(uint16_t _deviceData) : deviceData(_deviceData) { }

		bool operator<(const Trigger &right) const
		{
			if (button == right.button)
			{
				if (buttonState == right.buttonState)
					return deviceData < right.deviceData;
				else
					return buttonState < right.buttonState;
			}
			return button < right.button;
		}
	};

	struct EventMessage
	{
		string eventName;
		uint16_t mode;

		EventMessage(const string &_eventName, uint16_t _mode) : eventName(_eventName), mode(_mode) { }

		inline bool IsValidMode(InputMode _mode) const
        {
            return (mode & _mode) == _mode;
        }
	};

	/// Controls input within the engine, allowing for registration for events and polling
	class InputManager
    {
    private:
        // Singleton fun
        InputManager(void);
        InputManager(const InputManager&) {}
        InputManager &operator=(const InputManager &) {}

        static InputManager *instancePtr;

		// Input mode
		InputMode inputMode;

		// Data stores for input state
		map<pair<uint16_t,KeyData>, bool> keyboardState;
		MouseState mouseState;

		// Window handle for obtaining cursor position
		HWND hWnd;

		// Callback function for typed keys
		void (*typingCallback)(char);

		// Container for input event triggers
		map<Trigger, list<EventMessage>> keyboardTriggers;
		map<Trigger, list<EventMessage>> mouseTriggers;

        // Used to store the keydown events currently being fired each frame for keyboard
        std::set<std::string> firingEvents;

		map<std::string, uint16_t> keyCodeMapping;

		void BuildKeyCodeMapping(void);

    public:
        ~InputManager(void){}

		static InputManager &GetReference();	///< Get a reference to the InputManager singleton
        static void DeleteInstance();			///< Delete the instance of the InputManager singleton
		void SetWindowHandle(HWND _hWnd) { hWnd = _hWnd; } ///< Set a handle to the main window for obtaining cursor position

		/// Sets the callback function for typed input
		void SetTypingCallback(void (*_typingCallback)(char));
        // input :
        // typingCallback : pointer to function taking a single char parameter (the character typed)

		/// Add a keyboard event trigger, irrespective of keyboard location
		void AddKeyTrigger(const uint16_t vKey, ButtonState buttonState, const string &eventName, uint16_t mode = RAW_INPUT);
        // input :
        // vKey : virtual key code
        // keyState : State of the key (KEY_UP or KEY_DOWN)
		// eventName : Event to send when this trigger is activated

		/// Add a keyboard event trigger specific to one side of the keyboard
		void AddKeyTrigger(const uint16_t vKey, const ButtonState buttonState, const KeyData keyData, const string &eventName, uint16_t mode = RAW_INPUT);
        // input :
        // vKey : virtual key code
		// side : side of the keyboard (for multiple key codes like shift, alt, etc.)
        // keyState : State of the key (KEY_UP or KEY_DOWN)
		// eventName : Event to send when this trigger is activated

		/// Add a mouse event trigger
		void AddMouseTrigger(const uint16_t vKey, const ButtonState buttonState, const string &eventName, uint16_t mode = RAW_INPUT);
        // input :
        // flag : Mouse flag (mouse state change)
		// eventName : Event to send when this trigger is activated

		/// Add a mouse event trigger
		void AddMouseTrigger(const MouseData mouseData, const string &eventName, uint16_t mode = RAW_INPUT);
        // input :
        // flag : Mouse flag (mouse state change)
		// eventName : Event to send when this trigger is activated

		/// Post typed data to the InputManager
        void PostTypedEvent(WPARAM wParam, LPARAM lParam);

		/// Post raw key data to the InputManager
		void PostKeyEvent(const RAWKEYBOARD &rawKeyboard);

		/// Post raw mouse data to the InputManager
		void PostMouseEvent(const RAWMOUSE &rawMouse);

		/// Post an even using a specific trigger in the InputManager
		void PostMouseEvent(Trigger &trigger);

		/// Post an even using a specific trigger in the InputManager (with data)
		void PostMouseEvent(Trigger &trigger, void* data);

		// Post a cursor positioning data to the InputManager
		void PostMouseEvent(int32_t xPosition, int32_t yPosition);

		/// Get the input's current mode
		inline InputMode GetInputMode() { return inputMode; }

		/// Set the input's current mode
		inline void SetInputMode(InputMode mode) { inputMode = mode; }

		/// Get the current state of the mouse
		inline MouseState& GetMouseState() { return mouseState; }

		/// Get the current state of a key (side agnostic)
		inline bool IsKeyDown(uint16_t vKey) { return IsKeyDown(vKey, KEY_LEFT) || IsKeyDown(vKey, KEY_RIGHT); }

		/// Get the current state of a key (side specific)
		inline bool IsKeyDown(uint16_t vKey, KeyData keyData) { return keyboardState[make_pair(vKey, keyData)]; }

		/// Set the mouse's cursor position in window coordinates
		void SetCursorPosition(POINT position);

		/// Get the mouse's cursor position in window coordinates
		POINT GetCursorPosition();

		uint16_t GetKeyByName(std::string keyName) 
		{ 
			return keyCodeMapping[keyName]; 
		}

		ButtonState GetKeyStateByName(std::string stateName);
    };
}