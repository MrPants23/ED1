#pragma once

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace EDLUAWrappers
{

	typedef int (*LUAFunction)(lua_State *state);

	class LUAInterpreter
	{
		lua_State *state;

		LUAInterpreter();
		~LUAInterpreter();
	public:

		static LUAInterpreter &GetInstance();

		void RegisterFunction(const char *_functionName, LUAFunction _function);

		void RunScript(const char *_scriptFileName);

		lua_State* GetState(void)	{ return state; }

		void CallFunction(const char* _functionName);
	};
}