#include "RenderModel.h"
#include "RenderShapeWithTexture.h"
#include "RenderShapeTarget.h"
#include "RenderShapeGUI.h"
#include "RenderShapeDebugLines.h"
#include "RenderShapeLight.h"
using namespace EDRendererD3D;

#include <list>

#include "../tinyxml/tinyxml.h"
#include "../EDUtilities/InternalOutput.h"
#include "../EDUtilities/XMLLoadingMethods.h"

RenderModelData* RenderModelData::LoadXML( const char* xmlFileName, const char* indentifier )
{
	string temp = ContentManager::theContentPath;
	temp += xmlFileName;
	const char* xmlFileNamePtr = temp.c_str();

	InternalOutput::GetReference() << "Loading " << xmlFileNamePtr << "...\n";

	TiXmlDocument doc(xmlFileNamePtr);

	if(!doc.LoadFile())
	{
		InternalOutput::GetReference().Error(
			"Failed to load %s, does the file exist?\n", xmlFileNamePtr);
		return 0;
	}

	TiXmlHandle hDoc(&doc);
	TiXmlElement* elementPtr;
	TiXmlHandle hRoot(0);
		
	elementPtr = hDoc.FirstChildElement().Element();
	if( !elementPtr )
	{
		InternalOutput::GetReference().Error(
			"Failed to load %s, is the file empty?\n", xmlFileNamePtr);
		return 0;
	}

	std::string rootName = elementPtr->Value();

	if( strcmp(rootName.c_str(), "GDModel") != 0 )
	{
		InternalOutput::GetReference().Error(
			"Failed to load %s, missing a GDModel element\n", 
			xmlFileNamePtr);
		return 0;
	}

	TiXmlElement * pModelPartEle = elementPtr->FirstChildElement("GDModelPart");

	if( pModelPartEle == 0 )
	{
		InternalOutput::GetReference().Error(
			"Failed to load %s, missing a GDModelPart element\n", 
			xmlFileNamePtr);
		return 0;
	}


	RenderModelData* resultPtr = new RenderModelData;

	while( pModelPartEle != 0 )
	{
		RenderAtomicTransform atomicTransform;

		TiXmlElement* childElePtr = pModelPartEle->FirstChildElement("ParentIndex");

		if( childElePtr != 0 )
			if( childElePtr->GetText() != 0 )
				atomicTransform.parentIndex = atoi( childElePtr->GetText() );

		D3DXMatrixIdentity(&atomicTransform.localTransform);
		childElePtr = pModelPartEle->FirstChildElement("LocalTransform");
		
		if( childElePtr != 0 )
			ReadXmlMatrix( childElePtr, (float4x4 *)&atomicTransform.localTransform );

		
		childElePtr = pModelPartEle->FirstChildElement("GDFormFile");
		
		vector< ContentHandle<RenderForm> > newForms;

		while( childElePtr != 0 )
		{
			if( childElePtr->GetText() != 0 )
			{
				ContentHandle<RenderForm> newForm;
				std::string formPath;
				formPath += childElePtr->GetText();
				newForm = ContentManager::LoadXML<RenderForm>( formPath.c_str() );
				newForms.push_back(newForm);


			}
			childElePtr = childElePtr->NextSiblingElement("GDFormFile");

		}
		resultPtr->atomicTransforms.push_back( atomicTransform );
		resultPtr->renderFormHandles.push_back( newForms );

		pModelPartEle = pModelPartEle->NextSiblingElement("GDModelPart");
	}

	//std::vector< EDUtilities::Frame > tempFrames;
	//unsigned int numAtomics = resultPtr->atomicTransforms.size();
	//tempFrames.resize(numAtomics);
	//for(unsigned int i = 0; i < numAtomics; ++i)
	//{
	//	tempFrames[i].SetLocalMatrix( &resultPtr->atomicTransforms[i].localTransform ); 
	//	
	//	if( resultPtr->atomicTransforms[i].parentIndex != -1 )
	//		tempFrames[i].SetParent( &tempFrames[ resultPtr->atomicTransforms[i].parentIndex ] );		
	//}

	std::list< Sphere > tempSpheres;
	size_t numRenderForms = resultPtr->renderFormHandles.size();
	for(unsigned int i = 0; i < numRenderForms; ++i)
	{
        // base this off the highest resolution
		if(resultPtr->renderFormHandles[i][0].GetContent()->GetMeshHandle().GetContent() != 0)
		{
			Sphere currentSphere = resultPtr->renderFormHandles[i][0].GetContent()->GetMeshHandle().GetContent()->GetSphere();
			//currentSphere.Transform( tempFrames[i].GetWorldMatrix() );
			tempSpheres.push_back( currentSphere );
		}
	}

	if(tempSpheres.size() > 0)
		resultPtr->localSphere = tempSpheres.front();

	while( tempSpheres.size() > 1 )
	{
		std::list< Sphere >::iterator sphereA = tempSpheres.begin();
		std::list< Sphere >::iterator sphereB;

		std::list< Sphere >::iterator bestA;
		std::list< Sphere >::iterator bestB;

		Sphere bigSphere;
		bigSphere.center.makeZero();
		bigSphere.radius = FLT_MAX;

		for(; sphereA != tempSpheres.end(); ++sphereA )
		{
			sphereB = sphereA;
			++sphereB;

			for(; sphereB != tempSpheres.end(); ++sphereB)
			{
				Sphere testSphere;
				testSphere.Build( &(*sphereA), &(*sphereB) );

				if( testSphere.radius < bigSphere.radius )
				{
					bigSphere = testSphere;
					bestA = sphereA;
					bestB = sphereB;
				}
			}
		}

		tempSpheres.erase( bestB );
		tempSpheres.erase( bestA );

		tempSpheres.push_back( bigSphere );
		resultPtr->localSphere = bigSphere;
	}
	
//	if(tempSpheres.size() > 1)
	//	resultPtr->localSphere.InverseTransform( tempFrames[0].GetWorldMatrix() );

	return resultPtr;
}

RenderModel::~RenderModel(void)
{
	for(unsigned int i = 0; i < shapes.size(); ++i)
		delete shapes[i];
}

RenderModel::RenderModel(const char* xmlFileNamePtr)
{
	renderModelHandle = ContentManager::LoadXML<RenderModelData>( xmlFileNamePtr );

	if( renderModelHandle.GetContent() != 0 )
	{
		size_t numHandles = renderModelHandle.GetContent()->renderFormHandles.size();
		shapes.resize(numHandles);
		for( unsigned int i = 0; i < numHandles; ++i)
		{
			// Select the type of render shape that should be used
			switch(renderModelHandle.GetContent()->renderFormHandles[i][0].GetContent()->GetRenderShapeType())
			{
			case RST_Basic:
				shapes[i] = new RenderShape;
				break;
			case RST_WithMaterial:
				shapes[i] = new RenderShapeWithTexture;
				break;
			case RST_Target:
				shapes[i] = new RenderShapeTarget;
				break;
			case RST_GUI:
				shapes[i] = new RenderShapeGUI;
				break;
			case RST_DEBUG_LINES:
				shapes[i] = new RenderShapeDebugLines;
				break;
			case RST_Light:
				shapes[i] = new RenderShapeLight;
				break;
			};

			shapes[i]->Initialize( 
				renderModelHandle.GetContent()->renderFormHandles[i], 
				&renderModelHandle.GetContent()->atomicTransforms[i].localTransform );

			// Frame class is deprecated, as well as the current itteration of RenderModel...
			// RenderModel should still work for non-hierched mesh storage
			//if( renderModelHandle.GetContent()->atomicTransforms[i].parentIndex != -1 )
			//	shapes[i]->GetFramePtr()->SetParent( shapes[ renderModelHandle.GetContent()->atomicTransforms[i].parentIndex ]->GetFramePtr() );
		}
	}
}

const Sphere* RenderModel::GetSphere(void)
{
	if( renderModelHandle.GetContent() != 0 )
	{
		worldSphere = renderModelHandle.GetContent()->localSphere;
		worldSphere.Transform( *(float4x4 *)GetWorldMatrixPtr() );
		return &worldSphere;
	}
	else if( shapes.size() != 0 )
	{
		return &((RenderShape *)shapes[0])->GetSphere();
	}

	return 0;
}