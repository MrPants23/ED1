#include "MMHeap.h"

/*  Comment out the call to our Solution functions and implement your own.

	When you do this you also need to comment back in the multi threading
	protection.

	Don't worry we go over mutexes and synchronization later.
*/

namespace EDMemoryManager
{	
	Header * Heap::FindBlock(unsigned int allocSize)
	{
		if( freeListEntry == NULL )
		{
			return NULL;
		}
		else if(freeListEntry->size >= allocSize )
			return freeListEntry;
		else
		{
			Header* checkPoint = freeListEntry;
			checkPoint = checkPoint->next;
			while( checkPoint != freeListEntry )
			{
				if( checkPoint->size >= allocSize )
					return checkPoint;
				checkPoint = checkPoint->next;
			}
		}
		return NULL;
		//return FindBlockSolution(allocSize);
	}

	void Heap::Init(unsigned int poolSize, unsigned int _synchronize)
	{
		synchronize = _synchronize;
		InitializeCriticalSection(&criticalSection);
		MM_HEAP_LOCK;

		poolSizeTotal = poolSize;
		pool = (char*)malloc(poolSize);
		// Set the first header to the first 12 bytes of the pool;
		firstHeader = (Header*)pool;
		firstHeader->size = poolSize-sizeof(Header)-sizeof(Footer);
		firstHeader->next = firstHeader;
		firstHeader->prev = firstHeader;
		//firstHeader->size &= ~(1<<31);
		// Set the footer to the pool - the size of a footer
		lastFooter = (Footer*)((char*)pool + poolSize - sizeof(Footer));
		lastFooter->size = poolSize - sizeof(Header)-sizeof(Footer);
		//lastFooter->size &= ~(1<<31);

		freeListEntry = firstHeader;

		//InitSolution(poolSize, _synchronize);
	
		MM_HEAP_UNLOCK;
	}

	void * Heap::Allocate(unsigned int allocSize)
	{
		MM_HEAP_LOCK;

		/*void* dataPtr = AllocateSolution(allocSize);
		return dataPtr;*/
		Header* memToUse = FindBlock(allocSize);
		void* mem;
		if( memToUse->size >= allocSize+sizeof(Header)+sizeof(Footer)+1)
		{
			Footer* rightFoot = (Footer*)((char*)memToUse+sizeof(Header)+memToUse->size);
			Header* rightHead = (Header*)((char*)rightFoot-allocSize-sizeof(Header));
			Footer* leftFoot = (Footer*)((char*)rightHead-sizeof(Footer));

			mem = (char*)rightHead + sizeof(Header);

			// need to set size to allocsize
			rightHead->size = allocSize;
			rightFoot->size = allocSize;
			rightHead->size |= (1<<31);
			rightFoot->size |=(1<<31);

			unsigned int newSize = (unsigned int)(memToUse->size-allocSize-sizeof(Header)+sizeof(Footer));  // and header + footer

			memToUse->size = newSize;
			leftFoot->size = memToUse->size;
		}
		else
		{
			if( memToUse == freeListEntry )
			{
				// never null in cyclic list
				if( freeListEntry->next == NULL )
					freeListEntry = NULL;
				else
					freeListEntry = freeListEntry->next;
			}
			memToUse->prev->next = memToUse->next;
			memToUse->next->prev = memToUse->prev;
			Footer* pFoot = (Footer*)((char*)memToUse+sizeof(Header)+memToUse->size);
			mem = memToUse+sizeof(Header);
			(memToUse->size |= (1<<31));
			(pFoot->size |=(1<<31));
		}
		MM_HEAP_UNLOCK;
		return mem;
	}

	void Heap::DeAllocate(void * data)
	{
		//MM_HEAP_LOCK;

		//DeAllocateSolution(data);

		//MM_HEAP_UNLOCK;
	}
	
	unsigned int Heap::GetPoolSizeTotal()
	{
		return poolSizeTotal;
	}
}