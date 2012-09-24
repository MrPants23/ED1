//#if USE_MEMORY_MANAGER

//template <typename TYPE>
//void MMDel(TYPE * obj)
//{
//	if (obj)
//	{
//	obj->~TYPE();
//	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(obj);
//	}
//}
//
//template <typename TYPE>
//void MMDelEx(TYPE * obj, unsigned int heapId)
//{
//	if (obj)
//	{
//	obj->~TYPE();
//	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(obj,false,heapId);
//	}
//}
//
//template <typename TYPE>
//void MMDelArr(TYPE * obj)
//{
//	if (obj)
//	{
//	// Get the number of objects
//	unsigned int numObjects = *(((unsigned int *)obj)-2);
//	// Get the size of the objects that were actually allocated, so if a base class ptr gets delete called on it, we move by the size of the actual object when getting the next pointer.
//	unsigned int objSize = *(((unsigned int *)obj)-1);
//	char * iterObject = (char *)obj;
//	for (unsigned int i = 0; i < numObjects; ++i)
//	{
//		((TYPE *)iterObject)->~TYPE();
//		iterObject += objSize;
//	}
//	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(obj,true);
//	}
//}
//
//template <typename TYPE>
//void MMDelArrEx(TYPE * obj, unsigned int heapId)
//{
//	if (obj)
//	{
//	// Get the number of objects
//	unsigned int numObjects = *(((unsigned int *)obj)-2);
//	// Get the size of the objects that were actually allocated, so if a base class ptr gets delete called on it, we move by the size of the actual object when getting the next pointer.
//	unsigned int objSize = *(((unsigned int *)obj)-1);
//	char * iterObject = (char *)obj;
//	for (unsigned int i = 0; i < numObjects; ++i)
//	{
//		((TYPE *)iterObject)->~TYPE();
//		iterObject += objSize;
//	}
//	EDMemoryManager::MemoryManager::GetInstance()->DeAllocate(obj,true,heapId);
//	}
//}


//#if MM_LOG_MEMORY_LEAKS
//
//template <typename TYPE>
//void * MMArrNewFunc(unsigned int count, unsigned int heapId,char * fileName, int lineNum)
//{
//	unsigned int * ptr = (unsigned int *)EDMemoryManager::MemoryManager::GetInstance()->Allocate(sizeof(TYPE)*count,true,heapId,fileName,lineNum);
//#ifdef _DEBUG
//	ptr[0] = 0xFFFFFFFF; // pad so that after the number of elements the array data begins on a 16 byte boundary.
//	ptr[1] = 0xFFFFFFFF; // Filling in these values for debugging purposes.
//#endif
//	ptr[2] = count;
//	ptr[3] = sizeof(TYPE);
//	TYPE * iter = (TYPE *)(&(ptr[4]));
//	void * ret = iter;
//	for (unsigned int i = 0; i < count; ++i,++iter)
//	{
//		new (iter) TYPE; // Construct objects
//	}
//	return ret;
//}
//
//#else
//
//template <typename TYPE>
//void * MMArrNewFunc(unsigned int count, unsigned int heapId)
//{
//	unsigned int * ptr = (unsigned int *)EDMemoryManager::MemoryManager::GetInstance()->Allocate(sizeof(TYPE)*count,true,heapId);
//#ifdef _DEBUG
//	ptr[0] = 0xFFFFFFFF; // pad so that after the number of elements the array data begins on a 16 byte boundary.
//	ptr[1] = 0xFFFFFFFF; // Filling in these values for debugging purposes.
//#endif
//	ptr[2] = count;
//	ptr[3] = sizeof(TYPE);
//	TYPE * iter = (TYPE *)(&(ptr[4]));
//	void * ret = iter;
//	for (unsigned int i = 0; i < count; ++i,++iter)
//	{
//		new (iter) TYPE; // Construct objects
//	}
//	return ret;
//}

//#endif // MM_LOG_MEMORY_LEAKS
//
//#else // USE_MEMORY_MANAGER
//
//template <typename TYPE>
//void MMDel(TYPE * obj)
//{
//	delete obj;
//}
//
//template <typename TYPE>
//void MMDelEx(TYPE * obj, unsigned int heapId)
//{
//	delete obj;
//}
//
//template <typename TYPE>
//void MMDelArr(TYPE * obj)
//{
//	delete [] obj;
//}
//
//
//template <typename TYPE>
//void MMDelArrEx(TYPE * obj, unsigned int heapId)
//{
//	delete [] obj;
//}
//
//
//#endif // USE_MEMORY_MANAGER


