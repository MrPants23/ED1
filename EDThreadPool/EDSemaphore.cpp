#include "EDSemaphore.h"

namespace EDThreadPool
{
	EDSemaphore::EDSemaphore(long _numMax)
	{
		Init(_numMax);
	}

	EDSemaphore::~EDSemaphore()
	{
		Shutdown();
	}

	void EDSemaphore::Init(long _numMax)
	{
		// TODO - implement your own version
		//InitSolution(_numMax);
		semaphore = CreateSemaphore( NULL, 0, _numMax, NULL );
	}

	void EDSemaphore::Shutdown()
	{
		// TODO - implement your own version
		//ShutdownSolution();
		CloseHandle( semaphore );
	}

	void EDSemaphore::Wait(void)
	{
		// TODO - implement your own version
		//WaitSolution();
		WaitForSingleObject( semaphore, INFINITE );
	}

	bool EDSemaphore::TryWait(void)
	{
		// TODO - implement your own version
		//return TryWaitSolution();
		// State of the specified object is signaled
		if( WaitForSingleObject( semaphore, 5 ) == WAIT_OBJECT_0)
			return true;
		
		// If the object did not signal then false
		return false;
	}

	void EDSemaphore::Post(int _iPostAmount)
	{
		// TODO - implement your own version
		//PostSolution(_iPostAmount);
		ReleaseSemaphore( semaphore, _iPostAmount, NULL );
	}

} // end of EDThreadPool namespace
