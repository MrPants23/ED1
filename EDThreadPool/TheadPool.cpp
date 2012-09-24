#include "ThreadPool.h"

// these dependencies can be whacked when test objects are taken out for real tasks
#include "WorkerExample.h"
#include "../EDUtilities/InternalOutput.h"

namespace EDThreadPool
{
	ThreadPool* ThreadPool::instancePtr = 0;

	ThreadPool* ThreadPool::GetInstance()
	{
		if (instancePtr == 0)
		{
			instancePtr = new ThreadPool(DEFAULT_POOL_SIZE);
		}

		return instancePtr;
	}

	void ThreadPool::DeleteInstance()
	{
		if (instancePtr)
		{
			// if any test objects were created clear them out before we quit
			while(!instancePtr->testObjects.empty())
			{
				delete instancePtr->testObjects.front();
				instancePtr->testObjects.pop_front();
			}

			delete instancePtr;
			instancePtr = 0;
		}
	}

	void ThreadPool::CheckTestObjects()
	{
		std::list<WorkerThread*>::iterator iter;
		for(iter = testObjects.begin(); iter != testObjects.end(); iter++)
		{
			if ((*iter)->workDone.TryWait() == true)
			{
				EDUtilities::InternalOutput::GetReference() << "Thread Pool found " << ((WorkerExample*)(*iter))->primesFound << " primes between 1 and " << ((WorkerExample*)(*iter))->radix << "\n";
			}
		}
	}

	ThreadPool::ThreadPool(unsigned int _numThreads)
	{
		threadIDs = 0;
		Init(_numThreads);
	}

	ThreadPool::~ThreadPool()
	{
		Shutdown();
	}

	void ThreadPool::Init(unsigned int _numThreads)
	{
		// TODO - implement your own version
		//InitSolution(_numThreads);
		numThreads = _numThreads;
		threadIDs = new HANDLE[_numThreads];
		
		for( unsigned int i = 0; i < _numThreads; i++ )
		{
			threadIDs[i] = (HANDLE)_beginthreadex( NULL, 0, Execute, this, 0 , 0 );
		}
	}
	void ThreadPool::Shutdown()
	{
		// TODO - implement your own version
		//ShutdownSolution();
		if( shutdownType == DO_CURRENT_WORK )
		{
			queueMutex.Lock();
			while( !workers.empty() )
				workers.pop();
			queueMutex.Unlock();
		}
		runMutex.Lock();
		run = false;
		runMutex.Unlock();

		workCountSem.Post(numThreads);
		WaitForMultipleObjects( numThreads, threadIDs, TRUE, INFINITE );

		for( unsigned int i = 0; i < numThreads; i++ )
			CloseHandle( threadIDs[i] );

		delete[] threadIDs;
	}

	unsigned int __stdcall ThreadPool::Execute(void* _args)
	{
		// TODO - implement your own version
		//return ExecuteSolution(_args);
		ThreadPool* self = (ThreadPool*)_args;

		bool run = true;					// local run bool for completeing last bit of work
		WorkerThread* workerPtr = nullptr;	//worker thread
		int work = 0;						// amount of work left in the queue

		while( run )
		{
			workerPtr = nullptr;
			self->GetWork( &workerPtr );
			if( workerPtr != nullptr )
			{
				workerPtr->DoWork();
				workerPtr->workDone.Post();
			}
			self->runMutex.Lock();
			run = self->run;
			self->runMutex.Unlock();

			self->queueMutex.Lock();
			work = self->workers.size();
			self->queueMutex.Unlock();

			if( work > 0 )
				run = true;
		}
		
		/*self->runMutex.Lock();
		if(self->run == false)
		{
			
			while( self->workers.size() > 0 )
			{
				WorkerThread* workerPtr = nullptr;
				self->GetWork( &workerPtr );
				if( workerPtr != nullptr )
				{
					workerPtr->DoWork();
					workerPtr->workDone.Post();
				}
				else
					break;
			}
		}
		self->runMutex.Unlock();*/

		return self->shutdownType;
	}

	void ThreadPool::PostWork(WorkerThread* _workerPtr)
	{
		// TODO - implement your own version
		//PostWorkSolution(_workerPtr);
		queueMutex.Lock();
		workers.push( _workerPtr );
		queueMutex.Unlock();

		_workerPtr->workDone.TryWait();
		workCountSem.Post();
	}

	void ThreadPool::GetWork(WorkerThread** _workerPtr)
	{
		// TODO - implement your own version
		//GetWorkSolution(_workerPtr);
		workCountSem.Wait();
		queueMutex.Lock();
		{
			if( workers.size() > 0 )
			{
				*_workerPtr = workers.front();
				workers.pop();
			}
		}
		queueMutex.Unlock();
	}
} // end of EDThreadPool namespace
