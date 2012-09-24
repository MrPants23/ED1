#pragma once

#include "LUAInterpreter.h"

namespace EDLUAWrappers
{
	/// Create a sprite.
	///\param _state The lua interpreter that contains the following parameters:
	/// The filename of the model to serve as a sprite
	/// The x position in screen pixel coordinates to draw the sprite
	/// The y position in screen pixel coordinates to draw the sprite
	/// The width at which to draw the sprite
	/// The height at which to draw the sprite
	/// Rotation in radians (optional)
	/// Color modulation (optional, 4 floats)
	int CreateSprite(lua_State *_state);

	/// Load in sprites
	void LoadSprites(void);

	/// Reload sprites
	void ReloadSprites(void);
		
	/// Clean up sprite memory.
	void ReleaseSprites(void);

	/// Loops through the sprites and calls the appropriate LUA function
	void DrawSprites(void);
}