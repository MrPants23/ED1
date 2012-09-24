#include <list>
#include "RenderMesh.h"
#include "Renderer.h"
#include "VertexBufferManager.h"
#include "IndexBuffer.h"
#include "../EDMemoryManager/MMAllocator.h"
#include "../EDUtilities/GDMesh.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/ContentManager.h"
#include "../EDUtilities/GDMeshStream.h"

using namespace EDUtilities;
#include "atlstr.h"

namespace EDRendererD3D
{
    RenderMesh::RenderMesh()
    {
        numVertices = -1;
        startVertex = 0;
		startIndex = 0;
		//m_IsSubdivided = false;
		renderIndexRangePtr = 0;
		numRenderIndexRanges = 0;
	}

    RenderMesh::~RenderMesh()
    {
		delete []renderIndexRangePtr;
    }

	RenderMesh &RenderMesh::operator=(const RenderMesh & rhs)
	{
		if(this != &rhs)
		{
			numPrimitives = rhs.numPrimitives;
			primitiveType = rhs.primitiveType;
			startVertex = rhs.startVertex;
			startIndex = rhs.startIndex;
			numVertices = rhs.numVertices;
			
			boundingSphere = rhs.boundingSphere;
			boundingAabb = rhs.boundingAabb;

			numRenderIndexRanges = rhs.numRenderIndexRanges;
			delete [] renderIndexRangePtr;
			renderIndexRangePtr = new RenderIndexRange[numRenderIndexRanges]	;
			memcpy(renderIndexRangePtr, rhs.renderIndexRangePtr, 
				sizeof(RenderIndexRange) * numRenderIndexRanges);
		}
		return *this;
	}

	RenderMesh &RenderMesh::operator=(RenderMesh &&rhs)
	{
		if(this != &rhs)
		{
			numPrimitives = std::move(rhs.numPrimitives);
			primitiveType = std::move(rhs.primitiveType);
			startVertex = std::move(rhs.startVertex);
			startIndex = std::move(rhs.startIndex);
			numVertices = std::move(rhs.numVertices);
			
			boundingSphere = std::move(rhs.boundingSphere);
			boundingAabb = std::move(rhs.boundingAabb);

			numRenderIndexRanges = std::move(rhs.numRenderIndexRanges);
			delete [] renderIndexRangePtr;
			renderIndexRangePtr = std::move(rhs.renderIndexRangePtr); 
			rhs.renderIndexRangePtr = nullptr;
		}
		return *this;
	}

    void RenderMesh::CreateMesh(const VERTEX_POSCOLOR *verts, 
        unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType)
    {
        numVertices = numVerts;
        primitiveType = _primitiveType;
        numPrimitives = numVerts / 3;

        startVertex = VertexBufferManager::GetReference().GetPositionColorBuffer().AddVerts(verts, numVerts);
		
		boundingSphere.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POSCOLOR) );
		boundingAabb.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POSCOLOR) );
    }

    void RenderMesh::CreateMesh(const VERTEX_POSTEX *verts, 
        unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType)
    {
        numVertices = numVerts;
        primitiveType = _primitiveType;
        numPrimitives = numVerts / 3;

        startVertex = VertexBufferManager::GetReference().GetPositionTexBuffer().AddVerts(verts, numVerts);

		boundingSphere.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POSTEX) );
		boundingAabb.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POSTEX) );
    }
	
    void RenderMesh::CreateMesh(const VERTEX_POS *verts, 
        unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType)
    {
        numVertices = numVerts;
        primitiveType = _primitiveType;
        numPrimitives = numVerts / 3;

        startVertex = VertexBufferManager::GetReference().GetPositionBuffer().AddVerts(verts, numVerts);

		boundingSphere.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POS) );
		boundingAabb.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POS) );
    }

    void RenderMesh::CreateMesh(const VERTEX_POSNORMTEX *verts, 
        unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType)
    {
        numVertices = numVerts;
        primitiveType = _primitiveType;
        numPrimitives = numVerts / 3;

        // create vertex buffer
		startVertex = VertexBufferManager::GetReference().GetPosNormTexBuffer().AddVerts(verts, numVerts);

		boundingSphere.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POSNORMTEX) );
		boundingAabb.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POSNORMTEX) );
    }

    void RenderMesh::CreateMesh(const VERTEX_POSNORMTANTEX *verts, 
        unsigned int numVerts, D3DPRIMITIVETYPE _primitiveType)
    {
        numVertices = numVerts;
        primitiveType = _primitiveType;
        numPrimitives = numVerts / 3;

        // create vertex buffer
		startVertex = VertexBufferManager::GetReference().GetPosNormTanTexBuffer().AddVerts(verts, numVerts);

		boundingSphere.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POSNORMTANTEX) );
		boundingAabb.Build( &verts[0].position.x, numVerts, sizeof(VERTEX_POSNORMTANTEX) );
    }

	void RenderMesh::CreateIndexedMesh(const VERTEX_POSTEX *verts, 
        unsigned int numVerts, const unsigned int *indices, unsigned int numIndices, 
        D3DPRIMITIVETYPE primitiveType)
    {
        CreateMesh(verts, numVerts, primitiveType);
        numPrimitives = numIndices / 3;

        startIndex = IndexBuffer::GetReference().AddIndices(indices, numIndices);
    }

    void RenderMesh::CreateIndexedMesh(const VERTEX_POSNORMTEX *verts, 
        unsigned int numVerts, const unsigned int *indices, unsigned int numIndices, 
        D3DPRIMITIVETYPE primitiveType)
    {
        CreateMesh(verts, numVerts, primitiveType);
        numPrimitives = numIndices / 3;

        startIndex = IndexBuffer::GetReference().AddIndices(indices, numIndices);
    }

    void RenderMesh::CreateIndexedMesh(const VERTEX_POSNORMTANTEX *verts, 
        unsigned int numVerts, const unsigned int *indices, unsigned int numIndices, 
        D3DPRIMITIVETYPE primitiveType)
    {
        CreateMesh(verts, numVerts, primitiveType);
        numPrimitives = numIndices / 3;

        startIndex = IndexBuffer::GetReference().AddIndices(indices, numIndices);
    }

    void RenderMesh::CreateIndexedMesh(const VERTEX_POS *verts, 
        unsigned int numVerts, const unsigned int *indices, unsigned int numIndices, 
        D3DPRIMITIVETYPE primitiveType)
    {
        CreateMesh(verts, numVerts, primitiveType);
        numPrimitives = numIndices / 3;

        startIndex = IndexBuffer::GetReference().AddIndices(indices, numIndices);
    }

    void RenderMesh::CreateIndexedMesh(const VERTEX_POSCOLOR *verts, 
        unsigned int numVerts, const unsigned int *indices, unsigned int numIndices, 
        D3DPRIMITIVETYPE primitiveType)
    {
        CreateMesh(verts, numVerts, primitiveType);
        numPrimitives = numIndices / 3;

        startIndex = IndexBuffer::GetReference().AddIndices(indices, numIndices);
    }


    void RenderMesh::Read(const char* filename)
    {
        GDMesh gdMesh(filename);

        if( gdMesh.GetNumVerts() == 0 )
        {
            InternalOutput::GetReference() << "\nERROR : Can't find the model named " << filename << "\n";
            return;	
        }

        const float* pVerts = (const float*)gdMesh.GetAttribute("Position_3f")->GetAttributeData();
        const float* pNorms = (const float*)gdMesh.GetAttribute("Normal_3f")->GetAttributeData();
        const float* pTexCoords = (const float*)gdMesh.GetAttribute("TexCoord0_2f")->GetAttributeData();


		 VERTEX_POSNORMTEX *pInterVerts = new VERTEX_POSNORMTEX[gdMesh.GetNumVerts()];
        int interIndex = 0;
        for(int i = 0; i < gdMesh.GetNumVerts(); ++i)
        {
            pInterVerts[i].position.x = pVerts[i * 3 + 0];
            pInterVerts[i].position.y = pVerts[i * 3 + 1];
            pInterVerts[i].position.z = pVerts[i * 3 + 2];

            pInterVerts[i].normal.x = pNorms[i * 3 + 0];
            pInterVerts[i].normal.y = pNorms[i * 3 + 1];
            pInterVerts[i].normal.z = pNorms[i * 3 + 2];

            pInterVerts[i].texcoord.x = pTexCoords[i * 2 + 0];
            pInterVerts[i].texcoord.y = 1-pTexCoords[i * 2 + 1];
        }

        CreateIndexedMesh(pInterVerts, gdMesh.GetNumVerts(),
            (const unsigned int*)(gdMesh.GetIndices()), static_cast<unsigned int>(gdMesh.GetNumIndices()));
        BuildSphere(boundingSphere, pVerts, gdMesh.GetNumVerts());

		delete[] pInterVerts;
    }

    //RenderMesh* RenderMesh::Load(const char* filename)
    //{
    //    RenderMesh* mesh = new RenderMesh;
    //    mesh->Read(filename);
    //    return mesh;
    //}

    void RenderMesh::CreateIndexedMesh(unsigned int numVerts, 
        D3DXVECTOR3 *positions, D3DXVECTOR3 *normals, 
        D3DXVECTOR2 *texCoords, const unsigned int *indices, 
        unsigned int numIndices, D3DPRIMITIVETYPE primitiveType)

    {
		VERTEX_POSNORMTEX *pInterVerts = new VERTEX_POSNORMTEX[numVerts];
        int interIndex = 0;
        for(unsigned int i = 0; i < numVerts; ++i)
        {
            pInterVerts[i].position = positions[i];

            pInterVerts[i].normal = normals[i];

            pInterVerts[i].texcoord = texCoords[i];
        }

        CreateIndexedMesh(pInterVerts, numVerts, indices, numIndices, primitiveType);

        BuildSphere(boundingSphere, ((FLOAT *)positions), numVerts);

		delete[] pInterVerts;
    }

	RenderMesh* RenderMesh::Load(const char* pInFileName, const char* pVertexFormat)
	{
		string temp = ContentManager::theContentPath;
		temp += pInFileName;
		const char* fileName = temp.c_str();

		if( pVertexFormat == 0 )
			return 0;

		if( fileName == 0 )
			return 0;

		InternalOutput::GetReference() << "Loading " << fileName << "...\n";

		ContentHandle<GDMeshStreamBinary> meshHandle = ContentManager::Load<GDMeshStreamBinary>(fileName);

		if( meshHandle.GetContent() == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file exist?\n", fileName);
			return 0;
		}

		ContentHandle<GDAttribute<Point>> pointHandle = meshHandle.GetContent()->ReadAttribute<Point>();
		if( pointHandle.GetContent() == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file define points properly?\n",
				fileName);
			return 0;
		}

		size_t numVerts = pointHandle.GetContent()->size();
		if( numVerts == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file define points properly?\n",
				fileName);
			return 0;
		}

		ContentHandle<GDAttribute<TriVertIndices>> indicesHandle = meshHandle.GetContent()->GetIndicesHandle();
		if( indicesHandle.GetContent() == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file define indices properly?\n",
				fileName);
			return 0;
		}

		ContentHandle<GDAttribute<BVHNode>> bvhNodeHandle = meshHandle.GetContent()->ReadAttribute<BVHNode>();

		RenderIndexRange* pIndexRanges = 0;
		size_t renderIndexRangeCount = 0;
		
		if( bvhNodeHandle.GetContent() != 0 )
		{
			std::list< RenderIndexRange > renderIndexRanges;

			size_t handleSize = bvhNodeHandle.GetContent()->size();
			for(unsigned int i = 0; i < handleSize; ++i)
			{
				if( bvhNodeHandle.GetContent()->operator[](i).isLeaf == true )
				{
					RenderIndexRange newRange = { bvhNodeHandle.GetContent()->operator[](i).indices[0] * 3, 
						static_cast<unsigned int>(bvhNodeHandle.GetContent()->operator[](i).indices.size()) };
					renderIndexRanges.push_back( newRange );
				}
			}

			renderIndexRangeCount = renderIndexRanges.size();
			if( renderIndexRangeCount > 0 )
			{
				pIndexRanges = new RenderIndexRange[renderIndexRangeCount];
				std::list< RenderIndexRange >::iterator rirIter = renderIndexRanges.begin();
				unsigned int counter = 0;
				for(; rirIter != renderIndexRanges.end(); ++rirIter, ++counter)
				{
					pIndexRanges[counter] = (*rirIter);
				}
			}
		}

		// Read or build sphere...

		if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POS] ) == 0 )
		{
			VERTEX_POS *pInterVerts = new VERTEX_POS[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;
			}


			RenderMesh* resultPtr = new RenderMesh;
			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3) );
			
			resultPtr->renderIndexRangePtr = pIndexRanges;
			resultPtr->numRenderIndexRanges = static_cast<unsigned int>(renderIndexRangeCount);

			delete[] pInterVerts;

			return resultPtr;
		}
		else if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POSCOLOR] ) == 0 )
		{
			ContentHandle<GDAttribute<Color>> colorHandle = meshHandle.GetContent()->ReadAttribute<Color>();

			VERTEX_POSCOLOR *pInterVerts = new VERTEX_POSCOLOR[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;

				if( colorHandle.GetContent() == 0 )
					pInterVerts[i].color = D3DCOLOR_RGBA( 255, 255, 255, 255 );
				else
					pInterVerts[i].color = D3DCOLOR_RGBA( int(255 * colorHandle.GetContent()->operator[](i).Red),
														int(255 * colorHandle.GetContent()->operator[](i).Green),
														int(255 * colorHandle.GetContent()->operator[](i).Blue),
														int(255 * colorHandle.GetContent()->operator[](i).Alpha) ); 
			}


			RenderMesh* resultPtr = new RenderMesh;
			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3 ));
			
			resultPtr->renderIndexRangePtr = pIndexRanges;
			resultPtr->numRenderIndexRanges = static_cast<unsigned int>(renderIndexRangeCount);

			delete[] pInterVerts;

			return resultPtr;

		}
		else if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POSTEX] ) == 0 )
		{
			ContentHandle<GDAttribute<UV>> texCoordHandle = meshHandle.GetContent()->ReadAttribute<UV>("map1");
			
			if( texCoordHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing texture coordinates.\n",
					fileName);
				return 0;
			}

			VERTEX_POSTEX *pInterVerts = new VERTEX_POSTEX[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;

				pInterVerts[i].texcoord.x = texCoordHandle.GetContent()->operator[](i).u;
				pInterVerts[i].texcoord.y = texCoordHandle.GetContent()->operator[](i).v;
			}

			// CreateIndexedMesh()...
			delete [] pInterVerts;
		}
		else if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POSNORMTEX] ) == 0 )
		{
			ContentHandle<GDAttribute<Normal>> normalHandle = meshHandle.GetContent()->ReadAttribute<Normal>();

			if( normalHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing normals.\n",
					fileName);
				return 0;
			}

			ContentHandle<GDAttribute<UV>> texCoordHandle = meshHandle.GetContent()->ReadAttribute<UV>("map1");

			if( texCoordHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing texture coordinates.\n",
					fileName);
				return 0;
			}

			VERTEX_POSNORMTEX *pInterVerts = new VERTEX_POSNORMTEX[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;

				pInterVerts[i].normal.x = normalHandle.GetContent()->operator[](i).x;
				pInterVerts[i].normal.y = normalHandle.GetContent()->operator[](i).y;
				pInterVerts[i].normal.z = normalHandle.GetContent()->operator[](i).z;

				pInterVerts[i].texcoord.x = texCoordHandle.GetContent()->operator[](i).u;
				pInterVerts[i].texcoord.y = 1.0f - texCoordHandle.GetContent()->operator[](i).v;
			}


			RenderMesh* resultPtr = new RenderMesh;
			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3 ));
			
			//if( bvhNodeHandle.GetContent() != 0 )
			//	resultPtr->m_IsSubdivided = true;
			resultPtr->renderIndexRangePtr = pIndexRanges;
			resultPtr->numRenderIndexRanges = static_cast<unsigned int>(renderIndexRangeCount);

			delete []pInterVerts;

			return resultPtr;
		}
		else if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POSNORMTANTEX] ) == 0 )
		{
			ContentHandle<GDAttribute<Normal>> normalHandle = meshHandle.GetContent()->ReadAttribute<Normal>();

			if( normalHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing normals.\n",
					fileName);
				return 0;
			}

			ContentHandle<GDAttribute<Tangent>> tangentHandle = meshHandle.GetContent()->ReadAttribute<Tangent>();
			ContentHandle<GDAttribute<Bitangent>> bitangentHandle = meshHandle.GetContent()->ReadAttribute<Bitangent>();

			if( tangentHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing tangents.\n",
					fileName);
				return 0;
			}

			ContentHandle<GDAttribute<UV>> texCoordHandle = meshHandle.GetContent()->ReadAttribute<UV>("map1");

			if( texCoordHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing texture coordinates.\n",
					fileName);
				return 0;
			}

			VERTEX_POSNORMTANTEX *pInterVerts = new VERTEX_POSNORMTANTEX[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;

				pInterVerts[i].normal.x = normalHandle.GetContent()->operator[](i).x;
				pInterVerts[i].normal.y = normalHandle.GetContent()->operator[](i).y;
				pInterVerts[i].normal.z = normalHandle.GetContent()->operator[](i).z;

				pInterVerts[i].tangent.x = tangentHandle.GetContent()->operator[](i).x;
				pInterVerts[i].tangent.y = tangentHandle.GetContent()->operator[](i).y;
				pInterVerts[i].tangent.z = tangentHandle.GetContent()->operator[](i).z;
				
//				pInterVerts[i].tangent.w = 1;

				pInterVerts[i].texcoord.x = texCoordHandle.GetContent()->operator[](i).u;
				pInterVerts[i].texcoord.y = 1.0f - texCoordHandle.GetContent()->operator[](i).v;
			}

			// TODO: intended and missing the delete[], or old comment...?
			// CreateIndexedMesh()...
			//MMDelArr(pInterVerts);

			// Calculate determinates based handed-ness of texture coordinates to support UV mirroring
			// The following code is written assuming indeces refer to triangle lists
			// The following code is also written while my brain is in a bad place
			const TriVertIndices *Tris = &indicesHandle.GetContent()->operator[](0);
			const size_t numTriangles = indicesHandle.GetContent()->size();
			for(size_t index = 0; index < numTriangles; ++index)
			{
				D3DXVECTOR2 a, b;
				float detSign;
				
				a = pInterVerts[Tris[index].index1].texcoord - pInterVerts[Tris[index].index0].texcoord;
				b = pInterVerts[Tris[index].index2].texcoord - pInterVerts[Tris[index].index0].texcoord;
				detSign = a.x * b.y - a.y * b.x;
				pInterVerts[Tris[index].index0].tangent.w = detSign > 0 ? -1.0f : 1.0f;
				
				a = pInterVerts[Tris[index].index2].texcoord - pInterVerts[Tris[index].index1].texcoord;
				b = pInterVerts[Tris[index].index0].texcoord - pInterVerts[Tris[index].index1].texcoord;
				detSign = a.x * b.y - a.y * b.x;
				pInterVerts[Tris[index].index1].tangent.w = detSign > 0 ? -1.0f : 1.0f;
				
				a = pInterVerts[Tris[index].index0].texcoord - pInterVerts[Tris[index].index2].texcoord;
				b = pInterVerts[Tris[index].index1].texcoord - pInterVerts[Tris[index].index2].texcoord;
				detSign = a.x * b.y - a.y * b.x;
				pInterVerts[Tris[index].index2].tangent.w = detSign > 0 ? -1.0f : 1.0f;
			}



			RenderMesh* resultPtr = new RenderMesh;
			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3 ));
			delete []pInterVerts;

			return resultPtr;
		}

		return 0;
	}

	bool RenderMesh::LoadXML(RenderMesh* resultPtr, const char *xmlFileName, 
        const char *pVertexFormat)
	{
		string temp = ContentManager::theContentPath;
		temp += xmlFileName;
		const char* xmlFileNamePtr = temp.c_str();

		if( pVertexFormat == 0 )
			return 0;

		if( xmlFileNamePtr == 0 )
			return 0;

		InternalOutput::GetReference() << "Loading " << xmlFileNamePtr << "...\n";

		ContentHandle<GDMeshStreamXML> meshHandle = ContentManager::LoadXML<GDMeshStreamXML>(xmlFileName);

		if( meshHandle.GetContent() == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file exist?\n", xmlFileNamePtr);
			return 0;
		}

		ContentHandle<GDAttribute<Point>> pointHandle = meshHandle.GetContent()->ReadAttribute<Point>();
		if( pointHandle.GetContent() == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file define points properly?\n",
				xmlFileNamePtr);
			return 0;
		}

		size_t numVerts = pointHandle.GetContent()->size();
		if( numVerts == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file define points properly?\n",
				xmlFileNamePtr);
			return 0;
		}

		ContentHandle<GDAttribute<TriVertIndices>> indicesHandle = meshHandle.GetContent()->GetIndicesHandle();
		if( indicesHandle.GetContent() == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, does the file define indices properly?\n",
				xmlFileNamePtr);
			return 0;
		}

		ContentHandle<GDAttribute<BVHNode>> bvhNodeHandle = meshHandle.GetContent()->ReadAttribute<BVHNode>();

		RenderIndexRange* pIndexRanges = 0;
		size_t renderIndexRangeCount = 0;
		
		if( bvhNodeHandle.GetContent() != 0 )
		{
			std::list< RenderIndexRange > renderIndexRanges;
			size_t handleSize = bvhNodeHandle.GetContent()->size();
			for(unsigned int i = 0; i < handleSize; ++i)
			{
				if( bvhNodeHandle.GetContent()->operator[](i).isLeaf == true )
				{
					RenderIndexRange newRange = { bvhNodeHandle.GetContent()->operator[](i).indices[0] * 3, 
						static_cast<unsigned int>(bvhNodeHandle.GetContent()->operator[](i).indices.size()) };
					renderIndexRanges.push_back( newRange );
				}
			}

			renderIndexRangeCount = renderIndexRanges.size();
			if( renderIndexRangeCount > 0 )
			{
				pIndexRanges = new RenderIndexRange[renderIndexRangeCount];
				std::list< RenderIndexRange >::iterator rirIter = renderIndexRanges.begin();
				unsigned int counter = 0;
				for(; rirIter != renderIndexRanges.end(); ++rirIter, ++counter)
				{
					pIndexRanges[counter] = (*rirIter);
				}
			}
		}

		// Read or build sphere...

		if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POS] ) == 0 )
		{
			VERTEX_POS *pInterVerts = new VERTEX_POS[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;
			}


			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3 ));
			
			resultPtr->renderIndexRangePtr = pIndexRanges;
			resultPtr->numRenderIndexRanges = static_cast<unsigned int>(renderIndexRangeCount);

			delete [] pInterVerts;

			return true;
		}
		else if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POSCOLOR] ) == 0 )
		{
			ContentHandle<GDAttribute<Color>> colorHandle = meshHandle.GetContent()->ReadAttribute<Color>();

			VERTEX_POSCOLOR *pInterVerts = new VERTEX_POSCOLOR[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;

				if( colorHandle.GetContent() == 0 )
					pInterVerts[i].color = D3DCOLOR_RGBA( 255, 255, 255, 255 );
				else
					pInterVerts[i].color = D3DCOLOR_RGBA( int(255 * colorHandle.GetContent()->operator[](i).Red),
														int(255 * colorHandle.GetContent()->operator[](i).Green),
														int(255 * colorHandle.GetContent()->operator[](i).Blue),
														int(255 * colorHandle.GetContent()->operator[](i).Alpha) ); 
			}


			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3 ));
			
			resultPtr->renderIndexRangePtr = pIndexRanges;
			resultPtr->numRenderIndexRanges = static_cast<unsigned int>(renderIndexRangeCount);

			delete []pInterVerts;

			return true;

		}
		else if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POSTEX] ) == 0 )
		{
			ContentHandle<GDAttribute<UV>> texCoordHandle = meshHandle.GetContent()->ReadAttribute<UV>("map1");
			
			if( texCoordHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing texture coordinates.\n",
					xmlFileNamePtr);
				return 0;
			}

			VERTEX_POSTEX *pInterVerts = new VERTEX_POSTEX[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;

				pInterVerts[i].texcoord.x = texCoordHandle.GetContent()->operator[](i).u;
				pInterVerts[i].texcoord.y = texCoordHandle.GetContent()->operator[](i).v;
			}

			// CreateIndexedMesh()...

			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3 ));
			
			resultPtr->renderIndexRangePtr = pIndexRanges;
			resultPtr->numRenderIndexRanges = static_cast<unsigned int>(renderIndexRangeCount);

			delete[] pInterVerts;

			return true;
		}
		else if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POSNORMTEX] ) == 0 )
		{
			ContentHandle<GDAttribute<Normal>> normalHandle = meshHandle.GetContent()->ReadAttribute<Normal>();

			if( normalHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing normals.\n",
					xmlFileNamePtr);
				return 0;
			}

			ContentHandle<GDAttribute<UV>> texCoordHandle = meshHandle.GetContent()->ReadAttribute<UV>("map1");

			if( texCoordHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing texture coordinates.\n",
					xmlFileNamePtr);
				return 0;
			}

			VERTEX_POSNORMTEX *pInterVerts = new VERTEX_POSNORMTEX[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;

				pInterVerts[i].normal.x = normalHandle.GetContent()->operator[](i).x;
				pInterVerts[i].normal.y = normalHandle.GetContent()->operator[](i).y;
				pInterVerts[i].normal.z = normalHandle.GetContent()->operator[](i).z;

				pInterVerts[i].texcoord.x = texCoordHandle.GetContent()->operator[](i).u;
				pInterVerts[i].texcoord.y = 1.0f - texCoordHandle.GetContent()->operator[](i).v;
			}


			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3 ));
			
			//if( bvhNodeHandle.GetContent() != 0 )
			//	resultPtr->m_IsSubdivided = true;
			resultPtr->renderIndexRangePtr = pIndexRanges;
			resultPtr->numRenderIndexRanges = static_cast<unsigned int>(renderIndexRangeCount);

			delete []pInterVerts;

			return true;
		}
		else if( strcmp( pVertexFormat, VertexFormatString[eVERTEX_POSNORMTANTEX] ) == 0 )
		{
			ContentHandle<GDAttribute<Normal>> normalHandle = meshHandle.GetContent()->ReadAttribute<Normal>();

			if( normalHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing normals.\n",
					xmlFileNamePtr);
				return 0;
			}

			ContentHandle<GDAttribute<Tangent>> tangentHandle = meshHandle.GetContent()->ReadAttribute<Tangent>();

			if( tangentHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing tangents.\n",
					xmlFileNamePtr);
				return 0;
			}

			ContentHandle<GDAttribute<UV>> texCoordHandle = meshHandle.GetContent()->ReadAttribute<UV>("map1");

			if( texCoordHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing texture coordinates.\n",
					xmlFileNamePtr);
				return 0;
			}

			VERTEX_POSNORMTANTEX *pInterVerts = new VERTEX_POSNORMTANTEX[numVerts];

			int interIndex = 0;
			for(size_t i = 0; i < numVerts; ++i)
			{
				pInterVerts[i].position.x = pointHandle.GetContent()->operator[](i).x;
				pInterVerts[i].position.y = pointHandle.GetContent()->operator[](i).y;
				pInterVerts[i].position.z = pointHandle.GetContent()->operator[](i).z;

				pInterVerts[i].normal.x = normalHandle.GetContent()->operator[](i).x;
				pInterVerts[i].normal.y = normalHandle.GetContent()->operator[](i).y;
				pInterVerts[i].normal.z = normalHandle.GetContent()->operator[](i).z;

				pInterVerts[i].tangent.x = tangentHandle.GetContent()->operator[](i).x;
				pInterVerts[i].tangent.y = tangentHandle.GetContent()->operator[](i).y;
				pInterVerts[i].tangent.z = tangentHandle.GetContent()->operator[](i).z;

				pInterVerts[i].texcoord.x = texCoordHandle.GetContent()->operator[](i).u;
				pInterVerts[i].texcoord.y = 1.0f - texCoordHandle.GetContent()->operator[](i).v;
			}

			// TODO: Missing delete[], or just an old comment?
			// CreateIndexedMesh()...
			//MMDelArr(pInterVerts);


			resultPtr->CreateIndexedMesh( pInterVerts, static_cast<unsigned int >(numVerts), 
				static_cast<unsigned int *>(indicesHandle.GetContent()->operator[](0).indices), static_cast<unsigned int >(indicesHandle.GetContent()->size()*3 ));
			delete []pInterVerts;

			return true;
		}

		return false;

	}

	RenderMesh* RenderMesh::LoadXML(const char *xmlFileName, 
        const char *pVertexFormat)
	{
		RenderMesh* resultPtr = new RenderMesh;
		if(!LoadXML(resultPtr, xmlFileName, pVertexFormat))
		{
			delete resultPtr;
			resultPtr = 0;
		}

		return resultPtr;
	}

	RenderMesh RenderMesh::LoadXMLObject(const char *xmlFileName, 
        const char *pVertexFormat)
	{
		RenderMesh result;
		LoadXML(&result, xmlFileName, pVertexFormat);

		return result;
	}

}