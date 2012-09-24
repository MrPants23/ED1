
#include "HeapLog.h"
#include <iomanip>
#include <fstream>
namespace EDMemoryManager
{
	HeapLog::HeapLog()
	{
		numberOfAllocations = 0;
		numberOfDeAllocations = 0;
		totalSizeAllocated = 0;

		InitializeCriticalSection(&criticalSection);
	}

	HeapLog::~HeapLog()
	{
		DeleteCriticalSection(&criticalSection);
	}

	void HeapLog::Add(void * block, MemBlockLogInfo & info)
	{
		if(block == nullptr)
		{
			return;
		}

		EnterCriticalSection(&criticalSection);

		allocatedBlocks[block] = info;

		LeaveCriticalSection(&criticalSection);

		totalSizeAllocated += info.size;
	}
	void HeapLog::Remove(void * block)
	{
		if(block == nullptr)
		{
			return;
		}

		EnterCriticalSection(&criticalSection);

		totalSizeAllocated -= allocatedBlocks[block].size;
		allocatedBlocks.erase(block);

		LeaveCriticalSection(&criticalSection);
	}

	void HeapLog::WriteLogFile(const char * heapName)
	{
		std::fstream file(LEAK_LOG_LOCATION, std::ios_base::out);
		if (!file.is_open())
		{
			file.close();
			file.clear();
			file.open(LEAK_LOG_LOCATION, std::ios_base::trunc | std::ios_base::out);
		}
		if (file.is_open())
		{
			file << "\n\n********************************************\n";
			file << "HeapName:\t" << heapName << '\n';
			file << "NumLeaks:\t" << allocatedBlocks.size() << '\n';
			file << "********************************************\n";
			file << "Address        Size    File\n";

			for (AllocatedBlocksType::iterator iter = allocatedBlocks.begin(); iter != allocatedBlocks.end(); ++iter)
			{
				file << "0x" << std::hex << std::setw(8) << std::setfill('0') << (*iter).first;
				file << " " << std::dec << std::setw(8) << std::setfill(' ') << (*iter).second.size << "    ";

				if((*iter).second.fileName[0] != '\0')
				{
					file << (*iter).second.fileName << ":" << (*iter).second.lineNum;
				}

				file << '\n';
			}

			file.close();
			file.clear();
		}
	}

	unsigned int HeapLog::GetTotalSizeAllocated()
	{
		return totalSizeAllocated;
	}
}