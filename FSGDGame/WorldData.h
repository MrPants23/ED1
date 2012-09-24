#pragma once
#include "../EDUtilities/Timer.h"

class Game;

class WorldData
{
    friend class GameBase;
	friend class Game;
	friend class ArchGame;

private:
    /// Timer object used to track world state times
    static EDUtilities::Timer timer;
    /// The time between frames
    static float m_fDeltaTime;
    /// The width of the screen in pixels.
	static UINT m_ScreenWidth;
    /// The height of the screen in pixels.  This value includes the file menu.
	static UINT m_ScreenHeight;

	static void SetScreenWidth(UINT screenWidth) { m_ScreenWidth = screenWidth; }
	static void SetScreenHeight(UINT screenHeight) { m_ScreenHeight = screenHeight; }
    static void UpdateDeltaTime()
    {
        m_fDeltaTime =  timer.GetElapsedTime();
	    timer.Reset();
    }
public:
    /// \return Returns the amount of time passed between frames.
    inline static float GetDeltaTime() { return m_fDeltaTime; }
    /// \return Returns the width of the screen in pixels.
	inline static UINT GetScreenWidth() { return m_ScreenWidth; }
    /// \return Returns the height of the screen in pixels.  This includes the file menu.
	inline static UINT GetScreenHeight() { return m_ScreenHeight; }
};
