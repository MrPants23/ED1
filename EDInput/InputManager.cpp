#include "../EDMemoryManager/MMAllocator.h"
#include "InputManager.h"
#include "../EDUtilities/InternalOutput.h"

#define RI_MOUSE_HORIZONTAL_WHEEL 0x0800

namespace EDInput
{
	/**** Non-member functions ****/
	void ToggleConsoleCallBack(unsigned int eventId, void *eventData, void *listener)
	{
		InputMode mode = InputManager::GetReference().GetInputMode();
		if(TYPED_INPUT == mode)
			InputManager::GetReference().SetInputMode(RAW_INPUT);
		else
			InputManager::GetReference().SetInputMode(TYPED_INPUT);
	}

	/**** Singleton functionality ****/
	InputManager *InputManager::instancePtr = nullptr;

    InputManager::InputManager(void) 
    {
        inputMode = RAW_INPUT;
    	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
            "ToggleConsole", 0, ToggleConsoleCallBack);
    }

    InputManager &InputManager::GetReference()
    {
        if(nullptr == instancePtr)
        {
            instancePtr = new InputManager();
			instancePtr->BuildKeyCodeMapping();
        }
        return *instancePtr;
    }
 
	void InputManager::DeleteInstance()
	{
		delete instancePtr;
        instancePtr = nullptr;
    }

	void InputManager::SetTypingCallback(void (*_typingCallback)(char))
	{
		typingCallback = _typingCallback;
	}

	/**** General engine interface ****/
	void InputManager::AddKeyTrigger(const uint16_t vKey, ButtonState buttonState, const string &eventName, uint16_t mode)
	{
		// Insert trigger for key. If there are two of the key, insert a trigger for both sides of the keyboard.
		AddKeyTrigger(vKey, buttonState, KEY_LEFT, eventName, mode);
		AddKeyTrigger(vKey, buttonState, KEY_RIGHT, eventName, mode);
	}

	void InputManager::AddKeyTrigger(const uint16_t vKey, const ButtonState buttonState, const KeyData keyData, const string &eventName, uint16_t mode)
	{
		// Insert trigger for key. This will only be triggered on one side of the keyboard.	
		keyboardTriggers[Trigger(vKey, buttonState, keyData)].push_back(EventMessage(eventName, mode));

		// If this is a key down event add an event to remove it when the key is up
        if(buttonState == INPUT_HELD)
        {
			keyboardTriggers[Trigger(vKey, INPUT_RELEASED, keyData)].push_back(EventMessage(eventName, mode));
		}
	}

	void InputManager::AddMouseTrigger(const uint16_t vKey, const ButtonState buttonState, const string &eventName, uint16_t mode)
	{
		// Insert a trigger for this mouse state.
		mouseTriggers[Trigger(vKey, buttonState)].push_back(EventMessage(eventName, mode));
		
		// If this is a key down event add an event to remove it when the key is up
        if(buttonState == INPUT_HELD)
        {
			mouseTriggers[Trigger(vKey, INPUT_RELEASED)].push_back(EventMessage(eventName, mode));
		}
	}

	void InputManager::AddMouseTrigger(const MouseData mouseData, const string &eventName, uint16_t mode)
	{
		// Insert a trigger for this mouse state.
		mouseTriggers[Trigger(mouseData)].push_back(EventMessage(eventName, mode));
	}

	/**** Platform-specific functions ****/
	void InputManager::PostTypedEvent(WPARAM wParam, LPARAM lParam)
	{
		typingCallback((char)wParam);
	}

	void InputManager::PostKeyEvent(const RAWKEYBOARD &keyData)
	{
		EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();

		// If this is a key up event...
		if (keyData.Flags & 0x01)
		{
			// Data we need: side, key, event type.
			Trigger trigger(keyData.VKey, INPUT_RELEASED, (keyData.Flags & RI_KEY_E0) ? KEY_LEFT : KEY_RIGHT);

			for (list<EventMessage>::iterator message = keyboardTriggers[trigger].begin(); message != keyboardTriggers[trigger].end(); message++)
			{
				// If this event was being triggered persistently, remove it.
				if (firingEvents.find(std::string(message->eventName)) != firingEvents.end())
				{
					firingEvents.erase(std::string(message->eventName));
					eventManager->DeletePersistentEvent(message->eventName);
				}
				// Otherwise, if the mode matches, post the event.
				else if (message->IsValidMode(inputMode))
				{
					eventManager->PostEvent(message->eventName, nullptr, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
				}
			}

			// Mark this key as being up.
			keyboardState[make_pair(keyData.VKey, trigger.keyData)] = false;
		}
		// If it's a key down message...
		else
		{
			// For each possible key message (excludes KEY_RELEASED), see if there are events that should be triggered.
			for (int state = INPUT_PRESSED; state <= INPUT_HELD; state++)
			{
				// Data we need: side, key, event type.
				Trigger trigger(keyData.VKey, (ButtonState) state, (keyData.Flags & RI_KEY_E0) ? KEY_LEFT : KEY_RIGHT);

				// If the key is already down, ignore KEY_PRESSED and KEY_HELD triggers.
				if ((state == INPUT_PRESSED || state == INPUT_HELD) && keyboardState[make_pair(keyData.VKey, trigger.keyData)])
					continue;

				for (list<EventMessage>::iterator message = keyboardTriggers[trigger].begin(); message != keyboardTriggers[trigger].end(); message++)
				{
					// Is this the right input mode for this event? If not, skip this message.
					if (!message->IsValidMode(inputMode))
						continue;

					// For pressed and repeated events, send out a one-time event.
					// For held keys, initiate persistent event and add it to the list of firing events.
					switch (trigger.buttonState)
					{
					case INPUT_PRESSED:
					case INPUT_REPEATED:
						eventManager->PostEvent(message->eventName, nullptr, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
						break;
					case INPUT_HELD:
						eventManager->PostEvent(message->eventName, nullptr, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_PERSISTENT);
						firingEvents.insert(std::string(message->eventName));
						break;
					}
				}
			}

			// Set the key as pressed.
			keyboardState[make_pair(keyData.VKey, (keyData.Flags & RI_KEY_E0) ? KEY_LEFT : KEY_RIGHT)] = true;
		}
	}

	void InputManager::PostMouseEvent(const RAWMOUSE &rawMouse)
    {
		EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();

		// Check for mouse for movement.
		if (rawMouse.lLastX != 0 || rawMouse.lLastY != 0)
		{
			PostMouseEvent(Trigger(POSITION_CHANGED));

			// Update the mouse mickeys.
			if (!(rawMouse.usFlags & MOUSE_MOVE_ABSOLUTE))
			{
				mouseState.xMickeys += rawMouse.lLastX;
				mouseState.yMickeys += rawMouse.lLastY;
			}
		}

		// Check for wheel movement.
		if (rawMouse.usButtonFlags & RI_MOUSE_WHEEL)
		{
			PostMouseEvent(Trigger(WHEEL_MOVED), (void *) rawMouse.usButtonData);
			mouseState.wheelDelta += (int16_t) rawMouse.usButtonData;
		}

		if (rawMouse.usButtonFlags & RI_MOUSE_HORIZONTAL_WHEEL)
		{
			PostMouseEvent(Trigger(H_WHEEL_MOVED), (void *) rawMouse.usButtonData);
			mouseState.hWheelDelta += (int16_t) rawMouse.usButtonData;
		}

		uint16_t vKeys[5] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };

		// For each button, check the up and down flags and build events appropriately.
		for (int buttonNo = 0; buttonNo < 5; buttonNo++)
		{
			int buttonData = (rawMouse.usButtonFlags >> (buttonNo * 2)) & 0x03;

			// If this is a button up event...
			if (buttonData & 0x02)
			{
				// Data we need: button number, event type.
				Trigger trigger(vKeys[buttonNo], INPUT_RELEASED);

				for (list<EventMessage>::iterator message = mouseTriggers[trigger].begin(); message != mouseTriggers[trigger].end(); message++)
				{
					// If this event was being triggered persistently, remove it.
					if (firingEvents.find(std::string(message->eventName)) != firingEvents.end())
					{
						firingEvents.erase(std::string(message->eventName));
						eventManager->DeletePersistentEvent(message->eventName);
					}
					// Otherwise, if the mode matches, post the event.
					else if (message->IsValidMode(inputMode))
					{
						eventManager->PostEvent(message->eventName, nullptr, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
					}
				}

				// Mark this button as being up.
				mouseState.isPressed[buttonNo] = false;
			}

			// If it's a key down message...
			else if (buttonData & 0x01)
			{
				// For each possible key message (excludes KEY_RELEASED), see if there are events that should be triggered.
				for (int state = INPUT_PRESSED; state <= INPUT_HELD; state++)
				{
					// Data we need: button, event type.
					Trigger trigger(vKeys[buttonNo], (ButtonState) state);

					// If the button is already down, ignore KEY_PRESSED and KEY_HELD triggers.
					if ((state == INPUT_PRESSED || state == INPUT_HELD) && mouseState.isPressed[buttonNo])
						continue;

					for (list<EventMessage>::iterator message = mouseTriggers[trigger].begin(); message != mouseTriggers[trigger].end(); message++)
					{
						// Is this the right input mode for this event? If not, skip this message.
						if (!message->IsValidMode(inputMode))
							continue;

						// For pressed and repeated events, send out a one-time event.
						// For held keys, initiate persistent event and add it to the list of firing events.
						switch (trigger.buttonState)
						{
						case INPUT_PRESSED:
						case INPUT_REPEATED:
							eventManager->PostEvent(message->eventName, nullptr, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
							break;
						case INPUT_HELD:
							eventManager->PostEvent(message->eventName, nullptr, EDEventManager::EM_PRIORITY_ABOVE_NORMAL, EDEventManager::EM_FLAG_PERSISTENT);
							firingEvents.insert(std::string(message->eventName));
							break;
						}
					}
				}

				// Mark this button as being down.
				mouseState.isPressed[buttonNo] = true;
			}
		}
	}

	void InputManager::PostMouseEvent(Trigger &trigger)
	{
		EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();

		// Consider any triggers for cursor position changes.
		for (list<EventMessage>::iterator message = mouseTriggers[trigger].begin(); message != mouseTriggers[trigger].end(); message++)
		{
			if (message->IsValidMode(inputMode))
				eventManager->PostEvent(message->eventName, nullptr, EDEventManager::EM_PRIORITY_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
		}
	}

		void InputManager::PostMouseEvent(Trigger &trigger, void* data)
	{
		EDEventManager::EventManager *eventManager = EDEventManager::EventManager::GetInstance();

		// Consider any triggers for cursor position changes.
		for (list<EventMessage>::iterator message = mouseTriggers[trigger].begin(); message != mouseTriggers[trigger].end(); message++)
		{
			if (message->IsValidMode(inputMode))
				eventManager->PostEvent(message->eventName, data, EDEventManager::EM_PRIORITY_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);
		}
	}

	void InputManager::PostMouseEvent(int32_t xPosition, int32_t yPosition)
	{
		PostMouseEvent(Trigger(CURSOR_MOVED));
	}

	/// Set the mouse's cursor position
	void InputManager::SetCursorPosition(POINT position)
	{
		ClientToScreen(hWnd, &position);
		SetCursorPos(position.x, position.y);
	}

	/// Get the mouse's cursor position
	POINT InputManager::GetCursorPosition()
	{
		POINT position;
		GetCursorPos(&position);
		ScreenToClient(hWnd, &position);
		return position;
	}

	ButtonState InputManager::GetKeyStateByName(std::string stateName)
	{
		ButtonState returnState;

		if("Released" == stateName)
		{
			returnState = INPUT_RELEASED;
		}
		else if("Pressed" == stateName)
		{
			returnState = INPUT_PRESSED;
		}
		else if("Repeated" == stateName)
		{
			returnState = INPUT_REPEATED;
		}
		else if("Held" == stateName)
		{
			returnState = INPUT_HELD;
		}

		return returnState;
	}


	void InputManager::BuildKeyCodeMapping(void)
	{
		// Map the letter keys to the letter strings
		std::string value("A");
		while(value[0] <= 'Z')
		{
			keyCodeMapping.insert(pair<std::string, uint16_t>(value, value[0]));
			++value[0];
		}

		// Map the virtual key codes to the string of the same text
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SPACE", VK_SPACE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LBUTTON", VK_LBUTTON));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_RBUTTON", VK_RBUTTON));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_CANCEL", VK_CANCEL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_MBUTTON", VK_MBUTTON));

#if(_WIN32_WINNT >= 0x0500)
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_XBUTTON1", VK_XBUTTON1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_XBUTTON2", VK_XBUTTON2));
#endif
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_BACK", VK_BACK));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_TAB", VK_TAB));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_CLEAR", VK_CLEAR));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_RETURN", VK_RETURN));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SHIFT", VK_SHIFT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_CONTROL", VK_CONTROL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_MENU", VK_MENU));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_PAUSE", VK_PAUSE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_CAPITAL", VK_CAPITAL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_KANA", VK_KANA));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_HANGEUL", VK_HANGEUL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_HANGUL", VK_HANGUL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_JUNJA", VK_JUNJA));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_FINAL", VK_FINAL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_HANJA", VK_HANJA));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_KANJI", VK_KANJI));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_ESCAPE", VK_ESCAPE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_CONVERT", VK_CONVERT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NONCONVERT", VK_NONCONVERT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_ACCEPT", VK_ACCEPT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_MODECHANGE", VK_MODECHANGE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SPACE", VK_SPACE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_PRIOR", VK_PRIOR));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NEXT", VK_NEXT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_END ", VK_END ));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_HOME", VK_HOME));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LEFT", VK_LEFT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_UP", VK_UP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_RIGHT", VK_RIGHT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_DOWN", VK_DOWN));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SELECT", VK_SELECT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_PRINT", VK_PRINT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_EXECUTE", VK_EXECUTE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SNAPSHOT", VK_SNAPSHOT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_INSERT", VK_INSERT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_DELETE", VK_DELETE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_HELP", VK_HELP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LWIN", VK_LWIN));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_RWIN", VK_RWIN));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_APPS", VK_APPS));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SLEEP", VK_SLEEP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD0", VK_NUMPAD0));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD1", VK_NUMPAD1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD2", VK_NUMPAD2));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD3", VK_NUMPAD3));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD4", VK_NUMPAD4));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD5", VK_NUMPAD5));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD6", VK_NUMPAD6));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD7", VK_NUMPAD7));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD8", VK_NUMPAD8));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMPAD9", VK_NUMPAD9));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_MULTIPLY", VK_MULTIPLY));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_ADD", VK_ADD));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SEPARATOR", VK_SEPARATOR));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SUBTRACT", VK_SUBTRACT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_DECIMAL", VK_DECIMAL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_DIVIDE", VK_DIVIDE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F1", VK_F1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F2", VK_F2));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F3", VK_F3));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F4", VK_F4));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F5", VK_F5));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F6", VK_F6));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F7", VK_F7));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F8", VK_F8));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F9", VK_F9));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F10", VK_F10));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F11", VK_F11));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F12", VK_F12));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F13", VK_F13));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F14", VK_F14));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F15", VK_F15));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F16", VK_F16));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F17", VK_F17));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F18", VK_F18));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F19", VK_F19));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F20", VK_F20));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F21", VK_F21));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F22", VK_F22));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F23", VK_F23));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_F24", VK_F24));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NUMLOCK", VK_NUMLOCK));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_SCROLL", VK_SCROLL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_NEC_EQUAL", VK_OEM_NEC_EQUAL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_FJ_JISHO", VK_OEM_FJ_JISHO));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_FJ_MASSHOU", VK_OEM_FJ_MASSHOU));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_FJ_TOUROKU", VK_OEM_FJ_TOUROKU));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_FJ_LOYA", VK_OEM_FJ_LOYA));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_FJ_ROYA", VK_OEM_FJ_ROYA));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LSHIFT", VK_LSHIFT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_RSHIFT", VK_RSHIFT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LCONTROL", VK_LCONTROL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_RCONTROL", VK_RCONTROL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LMENU", VK_LMENU));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_RMENU", VK_RMENU));

#if(_WIN32_WINNT >= 0x0500)
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_BROWSER_BACK", VK_BROWSER_BACK));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_BROWSER_FORWARD", VK_BROWSER_FORWARD));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_BROWSER_REFRESH", VK_BROWSER_REFRESH));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_BROWSER_STOP", VK_BROWSER_STOP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_BROWSER_SEARCH", VK_BROWSER_SEARCH));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_BROWSER_FAVORITES", VK_BROWSER_FAVORITES));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_BROWSER_HOME", VK_BROWSER_HOME));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_VOLUME_MUTE", VK_VOLUME_MUTE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_VOLUME_DOWN", VK_VOLUME_DOWN));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_VOLUME_UP", VK_VOLUME_UP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_MEDIA_NEXT_TRACK", VK_MEDIA_NEXT_TRACK));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_MEDIA_PREV_TRACK", VK_MEDIA_PREV_TRACK));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_MEDIA_STOP", VK_MEDIA_STOP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_MEDIA_PLAY_PAUSE", VK_MEDIA_PLAY_PAUSE));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LAUNCH_MAIL", VK_LAUNCH_MAIL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LAUNCH_MEDIA_SELECT", VK_LAUNCH_MEDIA_SELECT));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LAUNCH_APP1", VK_LAUNCH_APP1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_LAUNCH_APP2", VK_LAUNCH_APP2));
#endif
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_1", VK_OEM_1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_PLUS", VK_OEM_PLUS));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_COMMA", VK_OEM_COMMA));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_MINUS", VK_OEM_MINUS));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_PERIOD", VK_OEM_PERIOD));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_2", VK_OEM_2));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_3", VK_OEM_3));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_4", VK_OEM_4));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_5", VK_OEM_5));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_6", VK_OEM_6));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_7", VK_OEM_7));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_8", VK_OEM_8));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_AX", VK_OEM_AX));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_102", VK_OEM_102));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_ICO_HELP", VK_ICO_HELP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_ICO_00", VK_ICO_00));

#if(WINVER >= 0x0400)
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_PROCESSKEY", VK_PROCESSKEY));
#endif
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_ICO_CLEAR", VK_ICO_CLEAR));

#if(_WIN32_WINNT >= 0x0500)
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_PACKET", VK_PACKET));
#endif
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_RESET", VK_OEM_RESET));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_JUMP", VK_OEM_JUMP));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_PA1", VK_OEM_PA1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_PA2", VK_OEM_PA2));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_PA3", VK_OEM_PA3));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_WSCTRL", VK_OEM_WSCTRL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_CUSEL", VK_OEM_CUSEL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_ATTN", VK_OEM_ATTN));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_FINISH", VK_OEM_FINISH));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_COPY", VK_OEM_COPY));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_AUTO", VK_OEM_AUTO));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_ENLW", VK_OEM_ENLW));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_BACKTAB", VK_OEM_BACKTAB));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_ATTN", VK_ATTN));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_CRSEL", VK_CRSEL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_EXSEL", VK_EXSEL));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_EREOF", VK_EREOF));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_PLAY", VK_PLAY));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_ZOOM", VK_ZOOM));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_NONAME", VK_NONAME));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_PA1", VK_PA1));
		keyCodeMapping.insert(pair<std::string, uint16_t>("VK_OEM_CLEAR", VK_OEM_CLEAR));
	}
}
