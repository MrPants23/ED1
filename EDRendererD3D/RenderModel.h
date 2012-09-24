#pragma once

#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include "RenderForm.h"
#include "RenderShape.h"

#include "../EDUtilities/ContentManager.h"
using namespace EDUtilities;

namespace EDRendererD3D
{
	/// Implementation of a parented transform read from a model file
	struct RenderAtomicTransform
	{
		int parentIndex;
		D3DXMATRIX localTransform;
	};

	/// Implementation of a model read in from file
	class RenderModelData
	{
		friend class RenderModel;

		string name;

		/// Container of parented transforms
		vector<RenderAtomicTransform> atomicTransforms;
		/// Container of Container of renderforms that make up the model,
        /// The root container has a 1:1 with atomicTransforms
        /// The interior container represents LODs of the model part 
        /// from highest res to lowest
		vector< vector< ContentHandle<RenderForm> > > 
            renderFormHandles;

		Sphere localSphere;
	public:

		RenderModelData(void){}
		
		/// Loads the data for a model from file
		/// \return Returns the pointer to the new model data.
		/// \param xmlFileNamePtr - File path to the RenderModelDataNew to load from XML format.
		/// \param indentifier - A unique identifier, unused in this context. Exists only to match function prototype.
		static RenderModelData* LoadXML( const char* xmlFileNamePtr, const char* indentifier );
	};

	/// Implementation of an instance of a model
	class RenderModel
	{
		/// Handle to the model data read in from file
		ContentHandle<RenderModelData> renderModelHandle;
		
		/// Container of RenderShape instances for the parts of the model instance.
		std::vector<RenderShapeBase*> shapes;

		Sphere worldSphere;
	public:
		
		RenderModel(void){}

		~RenderModel(void);

		/// Creates a new RenderModel instance from the XML MDL file
		/// \param xmlFileNamePtr - File path to the model to load.
		RenderModel(const char* xmlFileNamePtr);

		/// \return Returns the frame of the first RenderShape
		D3DXMATRIX *GetWorldMatrixPtr(void)
		{ 
			return (shapes.size() == 0 ? 0 :shapes[0]->GetWorldMatrixPtr()); 
		}

		/// \return Returns the from of the shape at the specified index.
		/// \param index - The index to access.
		D3DXMATRIX *GetWorldMatrixPtrAt(unsigned int index)
        { 
            return (shapes.size() > index ? shapes[index]->GetWorldMatrixPtr() : 0);
        }

		void SetWorldMatrix(float4x4 &mat) { SetWorldMatrix(*(D3DXMATRIX *)&mat); }
		void SetWorldMatrix(D3DXMATRIX &mat) { SetWorldMatrix(&mat); }
		void SetWorldMatrix(D3DXMATRIX *mat) 
		{
			if(shapes.size() != 0)
				shapes[0]->SetWorldMatrix(mat);
		}

	    const D3DXMATRIX *GetWorldMatrixPtrAt(unsigned int index) const
        { 
            return (shapes.size() > index ? shapes[index]->GetWorldMatrixPtr() : 0);
        }

		/// \return Returns the bounding sphere of the first shape.
		const Sphere* GetSphere(void);
		
		/// \return Returns the collection of RenderShapes that make up the model instance.
		std::vector<RenderShapeBase*>& GetShapes(void){ return shapes; }

		const std::vector<RenderShapeBase*>& GetShapes(void) const { return shapes; }

		/// Helper function that adds all RenderShapes of this model to their context sets for rendering.
		void AddToContextSets()
		{
			size_t vSize = shapes.size();
			for(size_t i = 0; i < vSize; ++i)
				shapes[i]->GetRenderContext()->AddRenderNode( shapes[i] );
		}

	};
}