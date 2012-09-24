-- LoadSprite.lua
-- Loads in sprite information and passes it to the C++ code via
	-- DrawSprite(lua_State *_state).  Plan to add functionality to read
	-- data from a .lua file to avoid hardcoding

-- Add requires here
-- Save path
gszPath = package.path;
	-- Set the path to read from the correct directory
	-- This path is relative to the .exe
	package.path = package.path ..";../EDLUAWrappers/Lua Scripts/Utility/?.lua";
		require "TableIO";
		require "StrictStruct";
	package.path = gszPath;

-- Passes the sprite's data to C++
function LoadSpriteFiles()
	-- Table to hold all the CSprites
	tSprites = {};

	-- Load in the Pause sprite
	table.insert(tSprites, LoadTable("../EDLUAWrappers/Lua Scripts/GUI/Player1HUD.lua"));
	table.insert(tSprites, LoadTable("../EDLUAWrappers/Lua Scripts/GUI/Player2HUD.lua"));
	table.insert(tSprites, LoadTable("../EDLUAWrappers/Lua Scripts/GUI/Player3HUD.lua"));
	table.insert(tSprites, LoadTable("../EDLUAWrappers/Lua Scripts/GUI/Player4HUD.lua"));
	
	-- Loop through all the tables in tSprite and call CreateSprite on them
	for index, sprite in ipairs(tSprites) do	
		-- CreateSprite(lua_State* _state)
			-- Function takes XML filename, x-pos, y-pos, width, height, rotation,
				-- color (ARGB)
		CreateSprite(sprite.m_szFilename, sprite.m_szID, sprite.m_fPosX, 
			sprite.m_fPosY, sprite.m_fWidth, sprite.m_fHeight, 
			sprite.m_bHoriFlip, sprite.m_bVertiFlip, sprite.m_fRotation, 
			sprite.m_fAlpha, sprite.m_fRed, sprite.m_fGreen, sprite.m_fBlue,
			sprite.m_nLayer);
	end
end