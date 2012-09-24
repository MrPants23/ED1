#pragma once

#include "VertexFormats.h"
#include "Renderer.h"

namespace EDRendererD3D
{
	/// \param VertexFormat - One of the vertex structures declared in VertexFormats.h
	template <typename VertexFormat>
	/// A vertex buffer handling class.  Instances of this class can be used to share
	/// vertex buffers, reducing the number of times the vertex stream must be switched.
    /// Use the VertexBufferManager to access the instances of this class.
	class VertexBuffer
	{
        friend class VertexBufferManager;
	private:
		/// The vertex declaration to be used by this mesh.  Based on the vertex layout.
		IDirect3DVertexDeclaration9	   *vertexDeclarationPtr;
		/// The vertex buffer containing the vertices of this mesh.
		IDirect3DVertexBuffer9		   *vertexBufferPtr;

		VertexBuffer(void);
        VertexBuffer(const VertexBuffer&) {}
        VertexBuffer &operator=(const VertexBuffer&) { return *this; }

        /// The amount to increase the vertex buffer by when it is full.
		const static int theVertSizeIncrement = 1000;

        /// The current size, in vertices, of the vertex buffer
		UINT numVertices;
        /// The number of vertices currently in use in the vertex buffer.
		UINT numVerticesUsed;

        /// A specialized method used to create the vertex declaration based on VertexFormat
		void CreateVertexDeclaration();
		/// A method used to increase the size of the vertex buffer.
        /// This method is called when  the vertex buffer is too small to add new vertices.
        void IncreaseVerts(UINT numVerts);
	public:
		~VertexBuffer(void) 
		{
			if(vertexDeclarationPtr)
			{
				vertexDeclarationPtr->Release();
				vertexDeclarationPtr = 0;
			}
			if(vertexBufferPtr)
			{
				vertexBufferPtr->Release();
				vertexBufferPtr = 0;
			}
		}

        /// Initializes the vertex buffer and creates the vertex declaration
		void Initialize();

		/// Adds new vertices to the vertex buffer.
        /// \param verts - The vertices to be added.
        /// \param numVerts - The number of vertices to be added.
        /// \return Returns the location in the vertex buffer where the added 
        /// vertices begin.  This is needed for rendering.
        UINT AddVerts(const VertexFormat *verts, UINT numVerts);

		inline IDirect3DVertexDeclaration9 *GetVertexDeclaration() { return vertexDeclarationPtr; }
		inline IDirect3DVertexBuffer9 *GetVertexBuffer() 
		{ 
			return vertexBufferPtr; 
		}
	};
}
#include "VertexBuffer.hpp"