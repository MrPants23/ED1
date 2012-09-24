#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <queue>
#include <list>
#include <process.h>
#include <Windows.h>

#include "EDMutex.h"
#include "EDSemaphore.h"
#include "WorkerThread.h"

#define DEFAULT_POOL_SIZE		2

/*  The ThreadPool class is designed to accept work tasks
*	and assign the work to idle threads.  Subclass the
*	WorkerThread to create workers
*
*	If you flood the pool with tasks it can block on exit
*	based on what the shutdownType is
*/

namespace EDThreadPool
{
	// when shutting down this will tell the pool how much work to do
	enum ShutdownType { DO_ALL_WORK, DO_CURRENT_WORK };

	class ThreadPool
	{
	private:

		// the number of spawned threads
		unsigned int numThreads;
	
		// a lock to protect the work que
		EDMutex queueMutex;
		// a lock to protect keep running bool
		EDMutex runMutex;

		// a sem to keep track of work in the que
		EDSemaphore workCountSem;

		// a list of worker threads
		std::queue<WorkerThread*> workers;

		// an array of handles to the created threads
		HANDLE* threadIDs;

		// keep track if the threadpool needs to run
		bool run;

		// if the pool should finish all tasks on exit
		ShutdownType shutdownType;

	private:
	
		void InitSolution(unsigned int _numThreads);
		void ShutdownSolution();
		static unsigned int __stdcall ExecuteSolution(void* _args);
		void PostWorkSolution(WorkerThread* _workerPtr);
		void GetWorkSolution(WorkerThread** _workerPtr);

		ThreadPool(unsigned int _numThreads);
		static ThreadPool* instancePtr;

		// initializes internal members and creates threads
		void Init(unsigned int _numThreads);

		// cleans up allocated resources
		void Shutdown();

	public:

		~ThreadPool();

		// singleton interface
		static ThreadPool* GetInstance();
		static void DeleteInstance();

		// the work horse function for all threads to execute
		// since we are using a windows function to create the thread we need to declare
		// this function as a __stdcall (default for c++ is __cdecl)
		static unsigned int __stdcall Execute(void* _args);

		// increases the work count, adds work to the que and posts the work
		void PostWork(WorkerThread* _workerPtr);
		// waits for work to be posted, grabs a work item from the que
		void GetWork(WorkerThread** _workerPtr);

		// change if the pool waits for all tasks for finish on exit
		inline void SetShutdownMethod(ShutdownType _shutdownType) { shutdownType = _shutdownType; }

		// test object funcationality can be whacked when real tasks are offloaded to the pool
		// list to keep track of pool testing objects
		std::list<WorkerThread*> testObjects;
		// checks the status of test objects
		void CheckTestObjects();
	};

} // end of EDThreadPool namespace

#endif
