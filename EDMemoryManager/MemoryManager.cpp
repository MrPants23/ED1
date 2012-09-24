

#include "MemoryManager.h"
#include <assert.h>

namespace EDMemoryManager
{
	MemoryManager *MemoryManager::GetInstance()
	{
		static MemoryManager theInstance;
		return &theInstance;
	}

	MemoryManager::MemoryManager()
	{
		heaps = (HeapData *)malloc(sizeof(HeapData) * MM_MAX_NUM_HEAPS);

#if MM_LOG_MEMORY_LEAKS 
		for(int heapIndex = 0; heapIndex < MM_MAX_NUM_HEAPS; ++heapIndex)
		{
			// We must explicitly call the HeapLog constructor because the memory was malloc'd
			new (&heaps[heapIndex].log) HeapLog();
		}
#endif
		InitializeCriticalSection(&criticalSection);
		CreateHeap(32 * 1024 * 1024, "Main Game Thread Heap1");
	}

	MemoryManager::~MemoryManager()
	{
#if MM_LOG_MEMORY_LEAKS 
		for(int heapIndex = 0; heapIndex < MM_MAX_NUM_HEAPS; ++heapIndex)
		{
			DestroyHeap(heapIndex);
		}
#endif
		DeleteCriticalSection(&criticalSection);
	}

	unsigned int MemoryManager::CreateHeap(unsigned int heapSize, const char *heapName, unsigned int flags)
	{
		EnterCriticalSection(&criticalSection);
		unsigned int i = 0;
		for (; i < MM_MAX_NUM_HEAPS; ++i)
		{
			heaps[i].heap.Init(heapSize, flags & MM_SYNCHRONIZE);

			if(heapName)
			{
				memcpy(heaps[i].heapName,heapName, strlen(heapName) + 1);
			}

			heaps[i].threadId = GetCurrentThreadId();
			LeaveCriticalSection(&criticalSection);
			return i;	
		}
		LeaveCriticalSection(&criticalSection);
		assert(false);
		return UINT_MAX;
	}

	void MemoryManager::DestroyHeap(unsigned int heapId)
	{
		EnterCriticalSection(&criticalSection);

		unsigned int threadId = GetCurrentThreadId();
		for (unsigned int i = 0; i < MM_MAX_NUM_HEAPS; ++i)
		{
			if (threadId == heaps[i].threadId)
			{
#if MM_LOG_MEMORY_LEAKS
				heaps[i].log.WriteLogFile(heaps[i].heapName);

				// We need to explicitly call HeapLog destructor because the memory was malloc'd
				// TODO: Make sure this is actually destructing AllocatedBlocksType
				heaps[i].log.~HeapLog();
#endif
			}
		}
		LeaveCriticalSection(&criticalSection);
	}

	void *MemoryManager::Allocate(unsigned int allocSize, bool isArray, unsigned int heapId, const char *fileName, int lineNum)
	{
		// If we have an array, pad an extra 16 bytes so that it will start the data on a 16 byte boundary and have room to store
		// the number of items allocated within this pad space, and the size of the original data type so in a delete call we can move
		// the pointer by the appropriate size and call a destructor(potentially a base class destructor) on each element in the array
		if (isArray)
		{
			// TODO: This isn't actually used at the moment, isArray is always false
			allocSize += sizeof(unsigned int) * 4;
		}

		if (heapId)
		{
			if (heapId < MM_MAX_NUM_HEAPS)
			{
				void *ret = heaps[heapId].heap.Allocate(allocSize);
#if MM_LOG_MEMORY_LEAKS 
				MemBlockLogInfo info;
				if(fileName)
				{
					memcpy(info.fileName, fileName, strlen(fileName) + 1);
				}
				info.lineNum = lineNum;
				info.size = allocSize;
				heaps[heapId].log.Add(ret, info);
#endif
				return ret;
			}
			// Heap Not Valid
			assert(false);
			return nullptr;
		}

		const unsigned int curThreadId = GetCurrentThreadId();
		for (unsigned int heapIndex = 0; heapIndex < MM_MAX_NUM_HEAPS; ++heapIndex)
		{
			if (curThreadId == heaps[heapIndex].threadId)
			{
				void *mem = heaps[heapIndex].heap.Allocate(allocSize);
				if (mem)
				{
#if MM_LOG_MEMORY_LEAKS 
					MemBlockLogInfo info;
					if(fileName)
					{
						memcpy(info.fileName,fileName, strlen(fileName) + 1);
					}
					else
					{
						info.fileName[0] = '\0';
					}
					info.lineNum = lineNum;
					info.size = allocSize;
					heaps[heapId].log.Add(mem, info);
#endif

					return mem;
				}	
			}	
		}

		// No heap memory was available for this thread.
		//assert(false);
		return nullptr;
	}

	void MemoryManager::DeAllocate(void *data, bool isArray, unsigned int heapId)
	{
		if (!data)
		{
#ifdef _DEBUG
			// We cannot get file/line from delete operator. Standard headers only handle the case where new
			//   is defined as something else. If delete is defined as something else they all blow
			//   up, unless the delete macro is pushed/undefined/poped when including the common
			//   headers.
			OutputDebugStringA("Deallocation of a null pointer\n");
#endif
			return;
		}
		
		if (isArray)
		{
			// TODO: This isn't actually used at the moment, isArray is always false
			data = ((char *)data) - sizeof(unsigned int) * 4;
		}

		if (heapId)
		{
			if (heapId < MM_MAX_NUM_HEAPS)
			{
#if MM_LOG_MEMORY_LEAKS
				heaps[heapId].log.Remove(data);
#endif
				heaps[heapId].heap.DeAllocate(data);
				return;	
			}
			// Heap Not Valid
			assert(false);
		}

		int heapIndex = -1;
		for (int i = 0; i < MM_MAX_NUM_HEAPS; ++i)
		{
			if (data > heaps[i].heap.firstHeader && data < heaps[i].heap.lastFooter)
			{
				heapIndex = i;
				break;
			}	
		}
		//assert(heapIndex != -1);

#if MM_LOG_MEMORY_LEAKS
		heaps[heapIndex].log.Remove(data);
#endif
		heaps[heapIndex].heap.DeAllocate(data);
	}

	size_t MemoryManager::GetTotalMemoryUsed(unsigned int heapId)
	{
		return heaps[heapId].log.GetTotalSizeAllocated();
	}

	size_t MemoryManager::GetTotalMemoryAvailable(unsigned int heapId)
	{
		return heaps[heapId].heap.GetPoolSizeTotal();
	}
}

