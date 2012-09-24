-- LoadSprite.lua
-- Loads in sprite information and passes it to the C++ code via
	-- DrawSprite(lua_State *_state).  Plan to add functionality to read
	-- data from a .lua file to avoid hardcoding

-- Passes the sprite's data to C++
function LoadSpriteFiles()
	-- Rendering Test.png via DrawSprite(lua_State* _state)
	-- Function takes filename, x-pos, y-pos, width, height, rotation,
		-- color (ARGB)
	print("Made it here!");
	DrawSprite("GDModel/mdl_PauseScreen.xml", 100, 100, 256, 256);
end