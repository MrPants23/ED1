#include "IndexBuffer.h"
#include "Renderer.h"
#include "../EDMemoryManager/MemoryManager.h"
using namespace EDMemoryManager;

namespace EDRendererD3D
{
    IndexBuffer *IndexBuffer::instancePtr = 0;

    IndexBuffer::IndexBuffer(void)
    {
        indexBufferPtr = 0;
        numIndices = 0;
		numIndicesUsed = 0;
    }

    IndexBuffer::~IndexBuffer(void)
    {
        if(indexBufferPtr)
		{
			indexBufferPtr->Release();
			indexBufferPtr = 0;
		}
    }

    IndexBuffer &IndexBuffer::GetReference()
    {
        if(!instancePtr)
        {
			instancePtr = new IndexBuffer;
        }
        return *instancePtr;
    }

    void IndexBuffer::DeleteInstance()
    {
		delete instancePtr;
    }

    
	void IndexBuffer::IncreaseIndices(UINT numIndices)
    {
        // Implement a solution for the Renderer Lab
        return IncreaseIndicesSolution(numIndices);
    }

    UINT IndexBuffer::AddIndices(const UINT *indices, UINT numIndices)
    {
        // Implement a solution for the Renderer Lab
        return AddIndicesSolution(indices, numIndices);
    }
}