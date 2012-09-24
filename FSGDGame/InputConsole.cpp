#include "InputConsole.h"

#include "../EDUtilities/InternalOutput.h"

#include "../EDRendererD3D/Renderer.h"
using EDRendererD3D::Renderer;

#include "WorldData.h"
#include "../EDEventManager/EventManager.h"
#include "../EDMemoryManager/MemoryManager.h"

InputConsole *InputConsole::instancePtr = 0;

InputConsole::InputConsole(void) 
{ 
    inputString = "";
	completionString = "";
	isEnabled = false; 
	fontPtr = 0;
	lastRememberedIndex = -1;
	textColor = D3DCOLOR_COLORVALUE(1, 1, 1, 1);

	textRect.left = 2;
	textRect.right = WorldData::GetScreenWidth();
	textRect.top = WorldData::GetScreenHeight() - 100;
	textRect.bottom = textRect.top + 20;

	for(int index = 0; index < NumStringsRemembered; ++index)
	{
		rememberedStrings[index] = "";
	}
}

InputConsole &InputConsole::GetReference()
{
	if(0 == instancePtr)
	{
		instancePtr = new InputConsole;
		instancePtr->Initialize();
	}
	return *instancePtr;
}

void InputConsole::DeleteInstance()
{
	delete instancePtr;
	instancePtr = 0;
}

InputConsole::~InputConsole(void)
{
	if(0 != fontPtr)
	{
		fontPtr->Release();
		fontPtr = 0;
	}
}

void InputConsole::ToggleConsoleCallback(unsigned int eventId, void *eventData, void *listener)
{
	GetReference().isEnabled = !GetReference().isEnabled;
}

void InputConsole::CompleteConsoleStringCallback(unsigned int eventId, void *eventData, void *listener)
{
	InputConsole &console = GetReference();
	if(console.isEnabled)
	{
		console.inputString = console.completionString;
	}
}

void InputConsole::RememberConsoleStringCallback(unsigned int eventId, void *eventData, void *listener)
{
	InputConsole &console = GetReference();

	// Find which string we are currently on
	int foundIndex = -1;

	for(int index = 0; index < NumStringsRemembered; ++index)
	{
		if(console.inputString == console.rememberedStrings[index])
		{
			foundIndex = index;
			break;
		}
	}

	// test if we did not find the string
	if(-1 == foundIndex)
	{
		foundIndex = console.lastRememberedIndex;
	}
	else if(0 == foundIndex)
	{
		foundIndex = NumStringsRemembered-1;
	}
	else
	{
		--foundIndex;
	}
	console.inputString = console.rememberedStrings[foundIndex];

}

void InputConsole::Initialize()
{
	EDEventManager::EventManager::GetInstance()->RegisterForEvent("ToggleConsole", 0, 
		ToggleConsoleCallback);
	EDEventManager::EventManager::GetInstance()->RegisterForEvent("CompleteConsoleString", 0, 
		CompleteConsoleStringCallback);
	EDEventManager::EventManager::GetInstance()->RegisterForEvent("RememberConsoleString", 0, 
		RememberConsoleStringCallback);

	D3DXCreateFont(Renderer::theDevicePtr, 20, 0, 0, FW_BOLD, 0, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial", &fontPtr);

	float heightDiv = 32.0f;
	backgroundSprite.LoadFromFile("GDModel/mdl_InputConsole.xml", (float)WorldData::GetScreenWidth()/2,
		(float)WorldData::GetScreenHeight() - (float)WorldData::GetScreenHeight()/heightDiv, 
		(float)WorldData::GetScreenWidth(), (float)WorldData::GetScreenHeight()/(heightDiv/2));
}

void InputConsole::ProcessKey(char key)
{
	// Don't do anything when disabled and don't process the ~
	if(!isEnabled || '`' == key)
		return;

	if(VK_RETURN == key)
	{
        std::string dataString = "";
        std::string::size_type index = inputString.find_first_of(" ");
        if((std::string::size_type) -1 != index)
        {
            dataString = inputString.substr(index+1, inputString.length());
            inputString[index] = 0;
        }
		EDEventManager::EventManager *em = EDEventManager::EventManager::GetInstance();
		em->PostEvent(inputString.c_str(), (void *)dataString.c_str(), 
			EDEventManager::EM_PRIORITY_NORMAL, EDEventManager::EM_FLAG_IMMEDIATE);

		RememberInputString();

        inputString = "";
        completionString = "";

	}
	else if(VK_BACK == key)
	{
		if(inputString != "")
		{
			inputString.erase(inputString.end()-1);
		}
	}
	else
	{
		inputString += key;
		completionString = "";		
		set<const string>::iterator iter = registeredCommands.begin();
		set<const string>::iterator iterEnd = registeredCommands.end();
		for(; iter != iterEnd; ++iter)
		{
			if(0 == (*iter).find(inputString))
			{
				completionString = (*iter);
				break;
			}
		}
	}
}

void InputConsole::Render()
{
	if(!isEnabled)
		return;

	backgroundSprite.Draw();
    fontPtr->DrawTextA(0, completionString.c_str(), -1, &textRect, 0, 
		D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
    fontPtr->DrawTextA(0, inputString.c_str(), -1, &textRect, 0, textColor);
}

void InputConsole::TypingCallback(char typed)
{
	InputConsole &console = InputConsole::GetReference();

	if(console.GetEnabled())
	{
		console.ProcessKey(typed);
	}
}

unsigned int InputConsole::RegisterCommand(const string &eventName, void *listener, 
	EDEventManager::EM_CALLBACK callback)
{
	RegisterExistingCommand(eventName);

	return EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		eventName, listener, callback);
}

void InputConsole::RegisterExistingCommand(const string &eventName)
{
	registeredCommands.insert(eventName);
}

void InputConsole::RememberInputString()
{
	++lastRememberedIndex;
	if(lastRememberedIndex >= NumStringsRemembered)
		lastRememberedIndex = 0;
	rememberedStrings[lastRememberedIndex] = inputString;
}
