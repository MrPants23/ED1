////////////////////////////////////////////////////////////////////////////////////
// MemoryManager.h
//
// Author :		Derek D Hughes
//
// Purpose:		Memory Manager to pre-allocate all dynamic memory required by the game.
//				Used by all containers. Guarentees all memory returned will be aligned
//				to 16 byte boundaries.
//			
// Limitations:	All structures being dynamically allocated must be at 
//					least 4 bytes in size;
//				The aligments will only work on a target platform of 32-bit.
//					Modification would be needed to configure for a target
//					platform of 64-bit.
//
// Usage:		
//				Change MM_LOG_MEMORY_LEAKS to 1 to track memory leaks, however, this
//					will cause dynamic memory to be allocated during run-time to store
//					the logging information separately from the memory manager.
// TODO: Update this header doc
////////////////////////////////////////////////////////////////////////////////////

#pragma once 
#include "MMHeap.h"
#include "HeapLog.h"

#ifdef MM_EXPORTS
#define MM_API __declspec(dllexport)
#else
#define MM_API __declspec(dllimport)
#endif

#define USE_MEMORY_MANAGER 1
#define MM_MAX_NUM_HEAPS 1

#ifdef _DEBUG
#define MM_LOG_MEMORY_LEAKS 1
#else
#define MM_LOG_MEMORY_LEAKS 0
#endif

namespace EDMemoryManager
{
	struct HeapData
	{
		/// Actual heap memory manager
		Heap heap;
		/// Name of this heap
		char heapName[512];
		/// ID of the thread that created this heap
		unsigned int threadId;
		/// Heap log for this heap
		HeapLog log;
	};

	class MM_API MemoryManager
	{
	public:
		enum 
		{
			/// Specifies that the heap is thread safe
			MM_SYNCHRONIZE = 1,
		};
		static MemoryManager *GetInstance();
		~MemoryManager();

		/// Allocates a block of memory.
		/// \param allocSize - Size in bytes of the memory to allocate.
		/// \param isArray - Determins if the memory is beign allocated as an array (eg: new[] ).
		/// \param heapId - ID of the heap to allocate on.
		/// \param fileName - Name of the source file where allocation is taking place.
		/// \param lineNum - Line number of the soruce file where allocation is taking place.
		/// \return Pointer to allocated memory or nullpointer if failed to allocate.
		void *Allocate(unsigned int allocSize, bool isArray = false, unsigned int heapId = 0, const char *fileName = "", int lineNum = 0);

		/// Deallocates a block of memory
		/// \param data - Pointer to memory being deallocated
		/// \param isArray - Determins if the memory is beign deallocated as an array (eg: delete[] )
		/// \param heapId - ID of the heap to deallocate on
		void DeAllocate(void *data, bool isArray = false, unsigned int heapId = 0);

		/// Creates a new heap
		/// \param heapSize - Size of the heap in bytes to create. 
		/// \param heapName - Name of the heap being created.
		/// \param flags - UNUSED
		/// \return ID of the heap or UINT_MAX on failure.
		unsigned int CreateHeap(unsigned int heapSize = 32 * 1024 * 1024, const char *heapName = "UNIDENTIFIED_HEAP", unsigned int flags = 0);

		/// Destroys all heaps created by the current running thread.
		/// \param heapId - Unused 
		void DestroyHeap(unsigned int heapId = 0);

		/// Gets the total memory used by the specifed heap
		/// \param heapId - ID of heap to get memory usage from 
		/// \return Number of bytes allocated on the heap
		size_t GetTotalMemoryUsed(unsigned int heapId = 0);

		/// Gets the total available memory for the specifed heap
		/// \param heapId - ID of heap to get total available memory from 
		/// \return Total memory available on the heap
		size_t GetTotalMemoryAvailable(unsigned int heapId = 0);
	private:
		MemoryManager();
		MemoryManager(const MemoryManager &);
		MemoryManager &operator=(const MemoryManager &);

		/// m_Heaps must not be freed because ifstream allocates memory (stored in this heap) whenever a file
		///   is opened. This memory is accessed after all other objects (including static) have been destroyed (during atexit)
		HeapData *heaps;
		CRITICAL_SECTION criticalSection;
	};

}