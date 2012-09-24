// EDGame.cpp : Defines the entry point for the application.
//

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <shellapi.h> // CommandLineToArgvW
#include <windowsx.h>

#include "FSGDWinMain.h"
#include "../EDMemoryManager/MMAllocator.h"
#include "../EDEventManager/EventManager.h"
#include "../EDInput/InputManager.h"
#include "../EDInput/JoystickManager.h"
using namespace EDInput;
//using EDInput::InputManager;

#include "../EDUtilities/InternalOutput.h"
using namespace EDUtilities;
#include "GameBase.h"
#include "ArchGame.h"
#include "FSGDGame.h"
#include "WorldData.h"
#define MAX_LOADSTRING 100
#include <string>
#include "../EDMemoryManager//OverloadNew.h"

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND globalHWnd;
GameBase *globalGamePtr;
HINSTANCE   app;
HWND        wnd;

#ifdef _WIN64
// Raw input data
#define RIBSize  48
#else
#define RIBSize  40
#endif

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
const std::map<std::string, std::string> GetCommandLineArgs();

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    //LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadString(hInstance, IDC_EDGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Initialize instances from a single threaded environment since these GetInstance() methods are
	//   not thread safe.
	EDMemoryManager::MemoryManager *memoryManagerPointer = EDMemoryManager::MemoryManager::GetInstance();
    EDEventManager::EventManager *eventManagerPointer = EDEventManager::EventManager::GetInstance();

	const std::map<std::string, std::string> argMap = GetCommandLineArgs();
	if(argMap.count("game") > 0)
	{
		std::string gameName = argMap.at("game");
		
		if(gameName == "arch")
		{
			globalGamePtr = new ArchGame;
		}
		else if(gameName == "ed")
		{
			globalGamePtr = new Game;
		}
	}
	else
	{
		globalGamePtr = new Game;
	}

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDGAME));

	InputManager::GetReference().SetWindowHandle(globalHWnd);


    // Main message loop:
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            globalGamePtr->Update();
            globalGamePtr->Render();
            //SwapBuffers(g_hDC);
        }
    }

    InputManager::DeleteInstance();
	delete globalGamePtr;

    return (int) msg.wParam;
}

/// Parses the command line string and stores all arguments in a key:value map of
/// strings. All arguments must begin with a single dash character '-' and are case
/// sensitive. Keys and values are split between a single equals sign character '='.
/// Arguments without a value are mapped to an empty string for their value.
/// \return Key:Value map of command line arguments.
const std::map<std::string, std::string> GetCommandLineArgs()
{
	int argc = 0;
	LPWSTR *argv = NULL;
	std::map<std::string, std::string> argMap;

	// Easiest way to quickly convert the single string command line into argv/argc parts
	argv = CommandLineToArgvW(GetCommandLine(), &argc);
	if(argv == NULL)
	{
		printf("CommandLineToArgvW failed with error %d!", GetLastError());
		return argMap;
	}

	for(int argIndex = 0; argIndex < argc; ++argIndex)
	{
		std::wstring currentArg = argv[argIndex];

		// Only accepting arguments beginning with '-'
		if(currentArg.length() == 0 || currentArg[0] != '-')
		{
			continue;
		}

		std::wstring argNameW;
		std::wstring argValueW;

		// Key:Value is split between the '=' character. If no character exists then only use the Key
		//    and set the Value to the default string.
		int splitAt = static_cast<int>(currentArg.find('='));
		if(splitAt != -1)
		{
			// Name is everything between the '-' and '=' characters
			argNameW = currentArg.substr(1, splitAt-1);
			// Value is everything after the '=' character
			argValueW = currentArg.substr(splitAt+1, (currentArg.length()-splitAt));
		}
		else
		{
			// Name is everything after the '-' character
			argNameW = currentArg.substr(1, currentArg.length()-1);
		}

		if(argNameW.length() == 0)
		{
			continue;
		}

		// Not sure if we're supporting unicode arguments or not, so it's just converting over to
		//  standard single byte character strings since they're generally easier to deal with.
		std::string argName = std::string(argNameW.begin(), argNameW.end());
		std::string argValue = std::string(argValueW.begin(), argValueW.end());
		argMap[argName] = argValue;
	}

	LocalFree(argv);
	return argMap;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDGAME));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= NULL;//(HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EDGAME);
    wcex.lpszClassName	= L"Title";
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    globalHWnd = CreateWindow(L"Title", L"FSGD Engine", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, WorldData::GetScreenWidth(), WorldData::GetScreenHeight(), NULL, NULL, hInstance, NULL);

    if (!globalHWnd)
    {
        return FALSE;
    }

    globalGamePtr->Initialize();

    ShowWindow(globalHWnd, nCmdShow);
    UpdateWindow(globalHWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    static HGLRC hRC = NULL;

    switch (message)
    {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_CREATE:
        {
            // Raw inout
            RAWINPUTDEVICE Rid[2];

            // Keyboard
            Rid[0].usUsagePage = 1;
            Rid[0].usUsage = 6;
            Rid[0].dwFlags = 0;
            Rid[0].hwndTarget=NULL;

            // Mouse
            Rid[1].usUsagePage = 1;
            Rid[1].usUsage = 2;
            Rid[1].dwFlags = 0;
            Rid[1].hwndTarget=NULL;

            if (RegisterRawInputDevices(Rid,2,sizeof(RAWINPUTDEVICE))==FALSE)
                InternalOutput::GetReference() << "Error - Failed to create Raw Input devices. \n";
        }
        break;
    case WM_CHAR:
        {
			InputManager::GetReference().PostTypedEvent(wParam, lParam);
        }
        break;
    case WM_INPUT:
        {
            UINT bufferSize = RIBSize;
            BYTE rawInputBuffer[RIBSize];
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)rawInputBuffer, 
                &bufferSize, sizeof(RAWINPUTHEADER));

            RAWINPUT *raw = (RAWINPUT *)rawInputBuffer;

            if( raw->header.dwType == RIM_TYPEKEYBOARD)
            {
                InputManager::GetReference().PostKeyEvent(raw->data.keyboard);
            }
            if( raw->header.dwType == RIM_TYPEMOUSE)
            {
                InputManager::GetReference().PostMouseEvent(raw->data.mouse);
            }
        }
        break;
	case WM_MOUSEMOVE:
		InputManager::GetReference().PostMouseEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_DEVICECHANGE:  
		JoystickManager::GetReference().RefreshDevices();
		break;
    case WM_SIZE:
        globalGamePtr->Resize(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

