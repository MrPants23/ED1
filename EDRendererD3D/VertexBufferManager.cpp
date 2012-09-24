#include "VertexBufferManager.h"
#include "../EDMemoryManager/MemoryManager.h"
using namespace EDMemoryManager;

namespace EDRendererD3D
{
	VertexBufferManager *VertexBufferManager::instancePtr = 0;

	VertexBufferManager::VertexBufferManager(void)
	{
		positionBuffer.Initialize();
		positionColorBuffer.Initialize();
		positionTexBuffer.Initialize();
		posNormTexBuffer.Initialize();
		posNormTanTexBuffer.Initialize();
 	}


	VertexBufferManager::~VertexBufferManager(void)
	{
	}

	VertexBufferManager &VertexBufferManager::GetReference()
	{
		if(0 == instancePtr)
		{
			instancePtr = new VertexBufferManager;
		}
		return *instancePtr;
	}

	void VertexBufferManager::DeleteInstance()
	{
		delete instancePtr;
		instancePtr = 0;
	}
}