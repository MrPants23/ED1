#include "EDMutex.h"

namespace EDThreadPool
{
	EDMutex::EDMutex()
	{
		Init();
	}

	EDMutex::~EDMutex()
	{
		Shutdown();
	}

	void EDMutex::Init()
	{
		// TODO - implement your own version
		//InitSolution();
		InitializeCriticalSection(&criticalSection);
	}

	void EDMutex::Shutdown()
	{
		// TODO - implement your own version
		//ShutdownSolution();
		DeleteCriticalSection(&criticalSection);
	}

	void EDMutex::Lock()
	{
		// TODO - implement your own version
		//LockSolution();
		EnterCriticalSection(&criticalSection); 
	}

	void EDMutex::Unlock()
	{
		// TODO - implement your own version
		//UnlockSolution();
		LeaveCriticalSection(&criticalSection);
	}

} // end of EDThreadPool namespace
