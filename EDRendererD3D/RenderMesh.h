#pragma once

#include "VertexFormats.h"
#include "../EDCollision/Collision.h"
using namespace EDCollision;

namespace EDRendererD3D
{
	/// A static mesh implementation.
    /// RenderMesh instances use the VertexBuffer and IndexBuffer classes to store their buffers.
    class RenderMesh
    {
    private:
		/// The number of primitives defined in this mesh
        UINT                            numPrimitives;
		/// The type of primitive contained in this mesh such as D3DPT_TRIANGLELIST
        D3DPRIMITIVETYPE                primitiveType;
		/// The index location of the first vertex in the vertex buffer for this mesh.
		/// This currently always 0, but will be used once the vertex buffers are consolodated.
        UINT                            startVertex;
		/// The index location of the first index in the index buffer for this mesh.
		/// This currently always 0, but will be used once the index buffers are consolodated.
        UINT                            startIndex;
		/// The number of vertices in this mesh.
        UINT                            numVertices;
		/// A bounding volume surrounding this mesh.  
        /// This is local to the mesh and should not be transformed.
        Sphere                          boundingSphere;

		Aabb							boundingAabb;
		struct RenderIndexRange
		{
			unsigned int offset;
			unsigned int length;
		};

		RenderIndexRange* renderIndexRangePtr;
		unsigned int numRenderIndexRanges;

		static bool LoadXML(RenderMesh* out, const char* xmlFileNamePtr, const char* pVertexFormat);

    public:
		/// Initializes members to default states.
        RenderMesh();
		/// Releases D3D API objects.
        ~RenderMesh();

		RenderMesh &operator=(const RenderMesh &rhs);
		RenderMesh &operator=(RenderMesh &&rhs);

		/// Initializes the non-indexed mesh based on data passed in.
		/// Vertices contain a position and color.
		/// \param verts The vertices to copy to the vertex buffer.
		/// \param numVerts The number of vertices to copy to the vertex buffer.
		/// \param primitiveType The type of primitive represented by the vertices.
        void CreateMesh(const VERTEX_POSCOLOR *verts, unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType = D3DPT_TRIANGLELIST);
        /// Initializes the non-indexed mesh based on data passed in.
		/// Vertices contain a position, normal and texture coordinate.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
        void CreateMesh(const VERTEX_POSNORMTEX *verts, unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType = D3DPT_TRIANGLELIST);
        /// Initializes the non-indexed mesh based on data passed in.
		/// Vertices contain a position, normal and texture coordinate.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
        void CreateMesh(const VERTEX_POSNORMTANTEX *verts, unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType = D3DPT_TRIANGLELIST);
        /// Initializes the non-indexed mesh based on data passed in.
		/// Vertices contain a position and texture coordinate.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
		void CreateMesh(const VERTEX_POSTEX *verts, unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType = D3DPT_TRIANGLELIST);
        /// Initializes the non-indexed mesh based on data passed in.
		/// Vertices contain a position.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
		void CreateMesh(const VERTEX_POS *verts, unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType = D3DPT_TRIANGLELIST);
        /// Initializes the indexed mesh based on data passed in.
		/// Vertices that contain a position and texture coordinate.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param indices - The indices to copy to the index buffer.
		/// \param numIndices - The number of indices to copy to the index buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
        void CreateIndexedMesh(const VERTEX_POSTEX *verts, unsigned int numVerts,
            const unsigned int *indices, unsigned int numIndices, D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST); 
		/// Initializes the indexed mesh based on data passed in.
		/// Vertices that contain a position, normal and texture coordinate.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param indices - The indices to copy to the index buffer.
		/// \param numIndices - The number of indices to copy to the index buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
        void CreateIndexedMesh(const VERTEX_POSNORMTEX *verts, unsigned int numVerts,
            const unsigned  int *indices, unsigned int numIndices, D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST);
        /// Initializes the indexed mesh based on data passed in.
		/// Vertices that contain a position, normal and texture coordinate.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param indices - The indices to copy to the index buffer.
		/// \param numIndices - The number of indices to copy to the index buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
        void CreateIndexedMesh(const VERTEX_POSNORMTANTEX *verts, unsigned int numVerts,
            const unsigned int *indices, unsigned int numIndices, D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST);
        /// Initializes the indexed mesh based on data passed in.
		/// Vertices that contain a position.
		/// \param verts - The vertices to copy to the vertex buffer.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param indices - The indices to copy to the index buffer.
		/// \param numIndices - The number of indices to copy to the index buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
        void CreateIndexedMesh(const VERTEX_POS *verts, unsigned int numVerts,
            const unsigned int *indices, unsigned int numIndices, D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST);

		void CreateIndexedMesh(const VERTEX_POSCOLOR *verts, unsigned int numVerts,
			const unsigned int *indices, unsigned int numIndices, D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST);

		/// Initializes the indexed mesh based on data passed in.  Interleaves separate
		/// vertex streams.
		/// \param numVerts - The number of vertices to copy to the vertex buffer.
		/// \param positions - The vertex positions.
		/// \param normals - The vertex normals.
		/// \param texCoords - The vertex texture coordinates.
		/// \param indices - The indices to copy to the index buffer.
		/// \param numIndices - The number of indices to copy to the index buffer.
		/// \param primitiveType - The type of primitive represented by the vertices.
        void CreateIndexedMesh(unsigned int numVerts, D3DXVECTOR3 *positions, D3DXVECTOR3 *normals, 
            D3DXVECTOR2 *texCoords, const unsigned int *indices, unsigned int numIndices, D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST);

        inline unsigned int GetPrimitiveCount() const { return numPrimitives; }
        inline D3DPRIMITIVETYPE GetPrimitiveType() const { return primitiveType; }
        inline unsigned int GetStartVertex() const { return startVertex; }
        inline unsigned int GetStartIndex() const { return startIndex; }
        inline unsigned int GetVertCount() const { return numVertices; }
        inline const Sphere &GetSphere(void) const { return boundingSphere; }
		inline const Aabb &GetAabb(void) const { return boundingAabb; }
		//inline bool IsSubdivided(void) const { return m_IsSubdivided; }
		inline const RenderIndexRange* GetRenderIndexRanges(void)const{ return renderIndexRangePtr; }
		inline unsigned int GetRenderIndexRangeCount(void)const{ return numRenderIndexRanges; }
		

		/// Initializes the indexed mesh based on a file.
		/// \param fileName - The name of the file to read the mesh from.
        void Read(const char* fileName);

		/// Creates a new indexed mesh in heap memory based on a file.
		/// \param fileName - The name of the file to read the mesh from.
		/// \param pVertexFormat - A string denoting the vertex format to be used, must exist
        /// in the array defined in VertexFormats.cpp.
		/// \return Returns the mesh.  This mesh will need to be deleted.
        static RenderMesh* Load(const char* fileName, const char* pVertexFormat);

		/// Loads a RenderMesh from a specified XML file.
		/// \param xmlFileNamePtr - File path to the RenderMesh to load from XML format.
		/// \param pVertexFormat - A string denoting the vertex format to be used, must exist
        /// in the array defined in VertexFormats.cpp.
		static RenderMesh* LoadXML(const char* xmlFileNamePtr, const char* pVertexFormat);

		static RenderMesh LoadXMLObject(const char* xmlFileNamePtr, const char* pVertexFormat);

    };
}