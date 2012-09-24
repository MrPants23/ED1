#pragma once

namespace EDInput
{
	// This class holds data about the current state of the mouse.
	class MouseState
	{
		friend class InputManager;
		enum MouseButton { MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_X1, MOUSE_X2, MAX_BUTTONS };

	private:
		bool isPressed[MAX_BUTTONS];
		int32_t xMickeys, yMickeys;
		int16_t wheelDelta, hWheelDelta;

	public:
		MouseState() : xMickeys(0), yMickeys(0), wheelDelta(0), hWheelDelta(0)
		{
			for each (bool &entry in isPressed)
				entry = false;
		}

		/// Returns true if the left button is currently pressed
		bool IsPressed(MouseButton button)
		{
			return isPressed[button];
		}

		/// Returns the number of X mickeys received since the last call to this function
		int32_t getXMickeys()
		{
			long delta = xMickeys;
			xMickeys = 0;
			return delta;
		}

		/// Returns the number of Y mickeys received since the last call to this function
		int32_t getYMickeys()
		{
			long delta = yMickeys;
			yMickeys = 0;
			return delta;
		}

		/// Returns the changes in the mouse wheel received since the last call to this function
		int16_t getWheelDelta()
		{
			unsigned short delta = wheelDelta;
			wheelDelta = 0;
			return delta;
		}

		/// Returns the changes in the mouse wheel received since the last call to this function
		int16_t getHorizontalWheelDelta()
		{
			unsigned short delta = hWheelDelta;
			hWheelDelta = 0;
			return delta;
		}

		/// Returns the number of X mickeys received since the last call to the getXMickeys function
		int32_t peekXMickeys()
		{
			return xMickeys;
		}

		/// Returns the number of Y mickeys received since the last call to the getYMickeys function
		int32_t peekYMickeys()
		{
			return yMickeys;
		}

		/// Returns the changes in the mouse wheel received since the last call to the getWheelDelta function
		int16_t peekWheelDelta()
		{
			return wheelDelta;
		}

		/// Returns the changes in the mouse wheel received since the last call to the getWheelDelta function
		int16_t peekHorizontalWheelDelta()
		{
			return hWheelDelta;
		}
	};
}