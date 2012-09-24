#pragma once

#pragma push_macro("new")
#undef new
#include <d3dx9.h>
#pragma pop_macro("new")

#include <string>
using std::string;

#include <set>
using std::set;

#include "../EDRendererD3D/RenderSprite.h"
using namespace EDRendererD3D;
#include "../EDEventManager/EventManager.h"
using EDEventManager::EventManager;

/// This singleton is used receive text input used to fire events.
/// The input received should be the name of an event to fire.  
/// Any data received after a space is passed as data to the event.
class InputConsole
{
public:
	~InputConsole(void);

    /// Gets a reference to instance of the InputConsole.
	static InputConsole &GetReference();
    /// Deletes the instance to the InputConsole.
	static void DeleteInstance();

    /// This method is used to process input received.
    /// \param key - The VK code of the key that should be proccessed.  
    /// This is typically mapped to user input, like the keyboard.
	void ProcessKey(char key);
    /// Renders the string data of the input that has been received.
	void Render();

	inline void SetEnabled(bool enabled) { isEnabled = enabled; }
	inline bool GetEnabled() { return isEnabled; }
	static void TypingCallback(char typed);

	/// Registers an event to be triggered when command is received.
	/// \param eventName - Name of the event to register for as well as the command to trigger it
	/// \param listener - Object registering as a listener for this event.
	/// \param callback - Function to call whenever event is raised.
	/// \return Event Id of the eventName.
	unsigned int RegisterCommand(const string &eventName, void *listener, 
		EDEventManager::EM_CALLBACK callback);
	/// Adds a command to th console auto-complete list.  This command should already have an 
	/// event associated with it, otherwise use the RegisterCommand method.
	/// \param eventName - Name of the event that has already been added to the event manager
	void RegisterExistingCommand(const string &eventName);
private:
	// Singleton fun
	InputConsole(void);
	InputConsole(const InputConsole&) {}
	InputConsole &operator=(const InputConsole &) {}

	static InputConsole *instancePtr;

    /// Whether the console is currently open and receiving input.
	bool isEnabled;

    /// The current input that has been received.
    string inputString;
    string completionString;

    /// The D3D font used to render what input has been received.
	ID3DXFont *fontPtr;
    /// The location to render input text to the screen.
	RECT textRect;
    /// The color to render input text.
	D3DCOLOR textColor;

	/// The background GUI element
	RenderSprite backgroundSprite;

	set<const string> registeredCommands;

	/// The number of input strings to remember for inputing again
	const static int NumStringsRemembered = 5;
	/// The index to the last string remembered
	int lastRememberedIndex;
	/// The input strings to remember for inputing again
	string rememberedStrings[NumStringsRemembered];

	/// Helper method for updating the remembered input strings
	void RememberInputString();

    /// Initializes the console for first use.  This is called when the instance is created.
	void Initialize();

    /// An event used to toggle the console's enabled state.
	static void ToggleConsoleCallback(unsigned int eventId, void *eventData, void *listener);

	/// An event used to attempt to auto-complete the input string.
	static void CompleteConsoleStringCallback(unsigned int eventId, void *eventData, void *listener);

	/// An event used to set the input string to a remembered input string
	static void RememberConsoleStringCallback(unsigned int eventId, void *eventData, void *listener);
};
