#ifndef _EDSEMAPHORE_H_
#define _EDSEMAPHORE_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/*	EDSemaphore is a simple semaphore implementation
*/

#define BINARY_SEMAPHORE   1
#define NORMAL_SEMAPHORE   MAXLONG32

namespace EDThreadPool
{

	class EDSemaphore
	{
	private:

		// handle to a semaphore object
		HANDLE semaphore;

		void InitSolution(long _numMax);
		void ShutdownSolution();
		bool TryWaitSolution(void);
		void WaitSolution(void);
		void PostSolution(int _iPostAmount);

		// create and setup semaphore
		void Init(long _numMax);

		// clean up any resources allocated
		void Shutdown();

	public:

		EDSemaphore(long _iMaxCount = NORMAL_SEMAPHORE);
		~EDSemaphore();

		// wait should decrease the count on the semaphore object
		void Wait(void);

		// trywait attempts to wait on the semaphore, returns true if the lock was a success
		bool TryWait(void);

		// post should increase the count on the semaphore object 
		void Post(int _iPostAmount = 1);
	};

} // end of EDThreadPool namespace

#endif
