#include "LUAInterpreter.h"
#include "SpriteWrapper.h"
#include "../EDUtilities/InternalOutput.h"

namespace EDLUAWrappers
{
	// CS - Error printing function called from Lua
	int GetError(lua_State *_state)
	{
		// CS - Adding ASCII art to make errors more visible
		EDUtilities::InternalOutput::GetReference()
			<< "\n************************************************************\n"
			<< "Lua:  " << lua_tostring(_state, -1)
			<< "\n************************************************************\n";

		return 0;
	}
	
	LUAInterpreter::LUAInterpreter()
	{
		state = lua_open();
		luaL_openlibs(state);

		// CS - Register the error function
		RegisterFunction("GetError", &GetError);
	}

	LUAInterpreter::~LUAInterpreter()
	{		
		lua_close(state);
	}

	LUAInterpreter &LUAInterpreter::GetInstance()
	{
		static LUAInterpreter instance;
		return instance;
	}

	void LUAInterpreter::RegisterFunction(const char *_functionName, LUAFunction _function)
	{
		lua_register(state, _functionName, _function);
	}

	void LUAInterpreter::RunScript(const char *_scriptFileName)
	{
		// CS - Load the .lua file
		if(!luaL_loadfile(state, _scriptFileName))
		{
			// CS - Run the .lua file
			if(lua_pcall(state, 0, LUA_MULTRET, 0))
			{
				GetError(state);
			}
		}
		else
		{
			GetError(state);
		}
	}

	void LUAInterpreter::CallFunction(const char* _functionName)
	{
		lua_getglobal(state, _functionName);
		lua_pcall(state, 0, 0, 0);
	}
}