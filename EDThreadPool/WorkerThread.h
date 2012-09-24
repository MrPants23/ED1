#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_

/*	WorkerThread is an abstract base class the the user will
	need to sub class in order to provide the thread pool with
	some work to do.
*/

namespace EDThreadPool
{

	class WorkerThread
	{
	public:

		// binary semaphore to let the owner know if the work is done, owner of the
		// work can check the value of TryWait to see if the work has been completed
		// if TryWait succeeds it will decrement the sem so it will not be valid until more work is done
		EDSemaphore workDone;

		// need to use initializer list so we don't use default in constructor
		WorkerThread() : workDone(BINARY_SEMAPHORE) {}

		// child classes need to over ride this to do useful work
		virtual void DoWork(void) = 0;
	};

} // end of EDThreadPool namespace

#endif
