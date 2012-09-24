#pragma once

#include <map>
//#include <string>
#include"MMMallocAllocator.h"

#include <Windows.h>

#define LEAK_LOG_LOCATION "../Logs/Leaked Memory.txt"

namespace EDMemoryManager
{
	struct MemBlockLogInfo
	{
		char fileName[512];
		int lineNum;
		int size;
		MemBlockLogInfo & operator = (const MemBlockLogInfo &ref)
		{
			memcpy(fileName,ref.fileName, strlen(ref.fileName) + 1);
			lineNum = ref.lineNum;
			size = ref.size;
			return *this;
		}
	};

	class HeapLog
	{
		typedef std::map<void*, MemBlockLogInfo, std::less<void*>, MMMallocAllocator<void*>> AllocatedBlocksType;

		// TODO: temp
		unsigned int numberOfAllocations;
		unsigned int numberOfDeAllocations;
		unsigned int totalSizeAllocated;

		AllocatedBlocksType m_AllocatedBlocks;
		AllocatedBlocksType allocatedBlocks;

		CRITICAL_SECTION criticalSection;

	public:
		HeapLog();
		~HeapLog();
		void Add(void * block, MemBlockLogInfo & info);
		void Remove(void * block);
		void WriteLogFile(const char * heapName);

		unsigned int GetTotalSizeAllocated();
	};
}