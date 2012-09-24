namespace EDRendererD3D
{
	template <typename VertexFormat>
	VertexBuffer<VertexFormat>::VertexBuffer(void) 
	{
		numVertices = 0;
		numVerticesUsed = 0;
		vertexBufferPtr = 0;
	}

	template <typename VertexFormat>
	void VertexBuffer<VertexFormat>::IncreaseVerts(UINT numVerts)
	{
		UINT increments = (numVerts / theVertSizeIncrement) * theVertSizeIncrement + theVertSizeIncrement;
		numVertices += increments;

		IDirect3DVertexBuffer9 *pOldVertexBuff = vertexBufferPtr;

		Renderer::theDevicePtr->CreateVertexBuffer(numVertices*sizeof(VertexFormat), 0, 0,
            D3DPOOL_DEFAULT, &vertexBufferPtr, 0);

		if(pOldVertexBuff)
		{
			void *oldBuffer;
			pOldVertexBuff->Lock(0,	(numVertices - increments)*sizeof(VertexFormat), &oldBuffer, 0);		

			void *buffer;
			vertexBufferPtr->Lock(0, (numVertices - increments)*sizeof(VertexFormat), &buffer, 0);	
			memcpy(buffer, oldBuffer, (numVertices - increments)*sizeof(VertexFormat));

			pOldVertexBuff->Unlock();
			vertexBufferPtr->Unlock();

			pOldVertexBuff->Release();
		}
	}

	template <typename VertexFormat>
	void VertexBuffer<VertexFormat>::Initialize()
	{
		IncreaseVerts(1);
		CreateVertexDeclaration();
	}

	template <typename VertexFormat>
	UINT VertexBuffer<VertexFormat>::AddVerts(const VertexFormat *verts, UINT numVerts)
	{
		//  Save the location to copy to
		UINT vertLocation = numVerticesUsed;

		// Increase the total verts
		numVerticesUsed += numVerts;

		// Check if more need to be made
		if(numVerticesUsed > numVertices) 
			IncreaseVerts(numVerticesUsed - numVertices);

        void *buffer;
        vertexBufferPtr->Lock(vertLocation*sizeof(VertexFormat),	
            numVerts*sizeof(VertexFormat), &buffer, 0);								
		// copy data to buffer
        memcpy(buffer, verts, numVerts*sizeof(VertexFormat));
        vertexBufferPtr->Unlock();

		return vertLocation;
	}
}