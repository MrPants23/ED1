#include "SpriteWrapper.h"
#include <string>
#include <map>
#include "../EDRendererD3D/RenderSprite.h"

#define SPRITE_LAYERS 3

namespace
{
	//std::map<std::string, EDRendererD3D::RenderSprite*> sprites;
	std::map<std::string, EDRendererD3D::RenderSprite*> sprites;
}

namespace EDLUAWrappers
{	
	int CreateSprite(lua_State *_state)
	{
		int numArguments = lua_gettop(_state);
		
		if(numArguments < 5 || numArguments > 14)
		{
			lua_pushstring(_state, "Incorrect number of arguments for 'DrawTexture'");
			lua_error(_state);
			return 1;
		}

		if(!lua_isstring(_state, 1))
		{
			lua_pushstring(_state, "First parameter for 'DrawTexture' must be a file name");
			lua_error(_state);
			return 1;
		}

		if(!lua_isstring(_state, 2))
		{
			lua_pushstring(_state, "Second parameter must be an ID");
			lua_error(_state);
			return 1;
		}
		
		for(int i = 3; i <= numArguments; ++i)
		{
			if(!lua_isnumber(_state, i) && !lua_isboolean(_state, i))
			{
				std::string error = "Parameter " + i;
				error += " for 'DrawTexture' must be a number or bool";
				lua_pushstring(_state, error.c_str());
				lua_error(_state);
				return 1;
			}
		}

		std::string filename = lua_tostring(_state, 1);
		std::string id = lua_tostring(_state, 2);
		unsigned int layer = (unsigned int)lua_tonumber(_state, 14);

		if(sprites[id] == nullptr)
		{
			HRESULT hr = S_OK;
			float a, r, g, b;
			a = r = g = b = 1.0f;

			bool flipHori = lua_toboolean(_state, 7) != 0;
			bool flipVert = lua_toboolean(_state, 8) != 0;

			sprites[id] = new EDRendererD3D::RenderSprite();
			sprites[id]->LoadFromFile(filename,
				(float)lua_tonumber(_state, 3), (float)lua_tonumber(_state, 4),
				(float)lua_tonumber(_state, 5), (float)lua_tonumber(_state, 6),
				flipHori, flipVert);
			
			if(hr == S_FALSE)
			{
				delete sprites[id];
				sprites[id] = nullptr;
				lua_pushstring(_state, "Invalid model file for 'DrawTexture'");
				lua_error(_state);
				return 1;
			}

			if(numArguments > 8)
				sprites[id]->Rotate((float)lua_tonumber(_state, 9));
			if(numArguments > 9)
				a = (float)lua_tonumber(_state, 10);
			if(numArguments > 10)
				r = (float)lua_tonumber(_state, 11);
			if(numArguments > 11)
				g = (float)lua_tonumber(_state, 12);
			if(numArguments > 12)
				b = (float)lua_tonumber(_state, 13);
			sprites[id]->SetColor(r, g, b, a);
		}

		return 0;
	}

	void LoadSprites(void)
	{
		LUAInterpreter::GetInstance().RegisterFunction("CreateSprite", &CreateSprite);
		
		//CS - Script paths are relative to FSGDGame, NOT EDLUAWrappers
		LUAInterpreter::GetInstance().RunScript("..\\EDLUAWrappers\\Lua Scripts\\GUI\\LoadSprite.lua");
		LUAInterpreter::GetInstance().CallFunction("LoadSpriteFiles");
	}

	void ReloadSprites(void)
	{
		// CS - Remove all sprites from memory
		ReleaseSprites();
		// CS - Load in sprites from Lua
		LoadSprites();
	}
	
	void ReleaseSprites(void)
	{
		// CS - Release the sprite information
		//for(unsigned int i = 0; i < SPRITE_LAYERS; ++i)
		//{
			std::map<std::string, EDRendererD3D::RenderSprite *>::iterator iter;
			for(iter = sprites.begin(); iter != sprites.end(); ++iter)
			{
				delete iter->second;
			}

			sprites.clear();
		//}
	}

	int ReleaseSprites(lua_State *_state)
	{
		//for(unsigned int i = 0; i < SPRITE_LAYERS; ++i)
		//{
			std::map<std::string, EDRendererD3D::RenderSprite *>::iterator iter;
			for(iter = sprites.begin(); iter != sprites.end(); ++iter)
			{
				delete iter->second;
			}

			sprites.clear();
		//}

		return 0;
	}

	void DrawSprites(void)
	{
		// CS - Draw GUI
		//for(unsigned int i = 0; i < SPRITE_LAYERS; ++i)
		//{
			std::map<std::string, EDRendererD3D::RenderSprite *>::iterator iter;
			for(iter = sprites.begin(); iter != sprites.end(); ++iter)
			{
				iter->second->Draw();
			}
		//}
	}
}