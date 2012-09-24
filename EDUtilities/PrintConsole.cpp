#include "PrintConsole.h"
#include <stdio.h>

namespace EDUtilities
{
	static const char *PageNames[PrintConsole::PPN_Total] = { "Render", "Input", "Solutions", "Memory" };
	static const char *ClassNames[NUM_CLASSES] = { "ED1", "ED2", "SWE" };

	PrintConsole *PrintConsole::instancePtr = 0;

	PrintConsole &PrintConsole::GetReference()
	{
		if(0 == instancePtr)
		{
			instancePtr = new PrintConsole;
		}
		return *instancePtr;
	}
	void PrintConsole::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = 0;
	}

	PrintConsole::PrintConsole(void)
	{
		isEnabled = false;
		currentPage = PPN_Render;
        fontPtr = 0;

		currentClassNo = 0;
		currentLabNo = 0;
	}

	PrintConsole::~PrintConsole(void)
	{
        if(fontPtr)
        {
            fontPtr->Release();
            fontPtr = 0;
        }
	}

	void PrintConsole::Initialize(void)
	{
		fontPtr = NULL;
		char buffer[128];
		for(PrintPageNames i = (PrintPageNames)0; i < PPN_Total;
			i = (PrintPageNames)(i + 1))
		{
			sprintf_s(buffer, 128, "%s %d/%d", PageNames[i], i+1, PPN_Total);
			pages[i].SetPageTitle(buffer);
		}

		pages[PPN_ED1Solutions].AddClass(ClassNames[0], 6);
		pages[PPN_ED1Solutions].AddClass(ClassNames[1], 1);
		pages[PPN_ED1Solutions].AddClass(ClassNames[2], 1);
	}

	void PrintConsole::CreateRenderFont(IDirect3DDevice9 *pDevice)
	{
        D3DXCreateFont(pDevice, 20, 0, 0, FW_BOLD, 0, ANSI_CHARSET, 
            OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial", &fontPtr);
	}

	void PrintConsole::Render()
	{
		if(!isEnabled)
			return;

		if(currentPage != PPN_ED1Solutions)
		{
			pages[currentPage].Render(fontPtr);
			pages[currentPage].ClearStrings();
		}
		else
		{
			pages[PPN_ED1Solutions].RenderSolutions(fontPtr, 
				ClassNames[currentClassNo], currentLabNo);
		}
	}

	void PrintConsole::ToggleConsoleCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();
		console.isEnabled = !console.isEnabled;
	}

	void PrintConsole::NextPageCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();

		console.currentPage = ((PrintPageNames)(console.currentPage + 1)); 
		if(console.currentPage >= PPN_Total)
			console.currentPage = (PrintPageNames)(console.currentPage - PPN_Total);
	}

	void PrintConsole::PreviousPageCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();
		console.currentPage = ((PrintPageNames)(console.currentPage - 1)); 
		if(console.currentPage < 0)
			console.currentPage = (PrintPageNames)(console.currentPage + PPN_Total);
	}

	void PrintConsole::NextClassCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();
		if(++console.currentClassNo >= NUM_CLASSES)
			console.currentClassNo = 0;

		console.currentLabNo = 0;
	}

	void PrintConsole::NextLabCallBack(void * evnt, void * comp)
	{
		PrintConsole &console = PrintConsole::GetReference();
		if(++console.currentLabNo >= 
			(unsigned int)console.pages[PPN_ED1Solutions].GetNumSolutions(
			ClassNames[console.currentClassNo]))
		{
			console.currentLabNo = 0;
		}
	}

}