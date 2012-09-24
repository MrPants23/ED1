#pragma once
#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

namespace EDRendererD3D
{
    /// A singleton class used to hold the index buffers used for rendering indexed geometry.
    class IndexBuffer
    {
    private:
        IndexBuffer(void);
        IndexBuffer(const IndexBuffer &) {}
        IndexBuffer &operator=(const IndexBuffer &) { return *this; }

        /// the one and only instance.
        static IndexBuffer *instancePtr;

        /// The index buffer used for all indices.
        IDirect3DIndexBuffer9 *indexBufferPtr;

        /// The amount to increase the index buffer by when it is full.
        const static int theIndexSizeIncrement = 1000;

        /// The current size, in indices, of the index buffer
		UINT numIndices;
        /// The number of indices currently in use in the index buffer.
		UINT numIndicesUsed;

        /// A method used to increase the size of the index buffer.
        /// This method is called when  the index buffer is too small to add new indices.
		void IncreaseIndices(UINT numIndices);

    public:
        ~IndexBuffer(void);

        /// Gets a reference to the one and only IndexBuffer instance.
        static IndexBuffer &GetReference();

        /// Deletes the instance of the IndexBuffer.
        static void DeleteInstance();

        /// Adds new indices to the indexBuffer.
        /// \param indices - The indices to be added.
        /// \param numIndices - The number of indices to be added.
        /// \return Returns the location in the index buffer where the added 
        /// indices begin.  This is needed for rendering.
        UINT AddIndices(const UINT *indices, UINT numIndices);

        inline IDirect3DIndexBuffer9 *GetIndices() { return indexBufferPtr; }

        /// Solution declarations, ignore these
        UINT AddIndicesSolution(const UINT *indices, UINT numIndices);
	    void IncreaseIndicesSolution(UINT numIndices);
    };
}