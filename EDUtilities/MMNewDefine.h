#ifdef MM_AVAILABLE
#define MMStlType(TYPE) TYPE, MMAllocator<TYPE>
#include "../EDMemoryManager/MMAllocator.h"
#else
#define MMNew(TYPE) new TYPE
#define MMNewArr(TYPE, COUNT) new TYPE[COUNT]
#define MMDel(OBJ) delete OBJ
#define MMDelArr(OBJ) delete [] OBJ
#define MMStlType(TYPE) TYPE
#endif