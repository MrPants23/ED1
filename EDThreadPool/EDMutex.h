#ifndef _EDMUTEX_H_
#define _EDMUTEX_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/*	EDMutex is a simple mutex implementation using critical sections
	for thread synchronization
*/

namespace EDThreadPool
{

	class EDMutex
	{
	private:

		// critial section provides synchronization
		CRITICAL_SECTION criticalSection;

	private:
		void InitSolution();
		void ShutdownSolution();
		void LockSolution();
		void UnlockSolution();

		// initialize the critical section
		void Init();

		// clean up any resources allocated
		void Shutdown();

	public:

		EDMutex();
		~EDMutex();

		// lock will attempt to get ownership of critical section
		void Lock();

		// unlock should release ownership of the object
		void Unlock();
	};

} // end of EDThreadPool namespace

#endif
