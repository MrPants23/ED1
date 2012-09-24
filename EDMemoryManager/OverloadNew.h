#ifndef MY_MEMORY
#define MY_MEMORY
#pragma once

#include "MemoryManager.h"

#ifdef new
#undef new
#endif

inline void * operator new (size_t size)
{
	return EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size));
}

inline void * operator new (size_t size, const char *file, int line)
{
	return EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), false, 0U, file, line);
}

inline void operator delete (void * p)
{
	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(p, false);
}

// Unused
inline void operator delete (void * p, const char *file, int line)
{
	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(p, false);
}


// What was the reason as to why these couldn't be inlined?
// TODO: These have never been marked as 'isArray', breaks when they are set though
inline void * operator new[] (size_t size, const char *file, int line)
{
	return EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), false, 0U, file, line);
}

inline void * operator new[] (size_t size)
{
	return EDMemoryManager::MemoryManager::GetInstance()->Allocate(static_cast<unsigned int>(size), false);
}

inline void operator delete[] (void * p)
{
	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(p, false);
}

// Unused
inline void operator delete[] (void * p, const char *file, int line)
{
	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(p, false);
}

#if MM_LOG_MEMORY_LEAKS
#define new new( __FILE__, __LINE__)
//#define delete delete( __FILE__, __LINE__) // We can't have nice things.
#endif
#endif
 