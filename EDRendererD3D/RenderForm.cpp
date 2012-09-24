#include "RenderForm.h"
#include "RenderShape.h"
#include "RenderShapeDebugLines.h"
#include "RenderShapeGUI.h"
#include "RenderShapeLight.h"
#include "RenderShapeTarget.h"
using namespace EDRendererD3D;

#include "../tinyxml/tinyxml.h"
#include "VertexFormats.h"
#include "../EDUtilities/InternalOutput.h"

namespace EDRendererD3D
{
	RenderForm* RenderForm::LoadXML(const char* xmlFileName, const char* indentifier)
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

		if( strcmp(rootName.c_str(), "GDForm") != 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a GDForm element.\n",
				xmlFileNamePtr);
			return 0;
		}

		std::string filePath = ContentManager::theContentPath;

		TiXmlElement* pRCXElement = elementPtr->FirstChildElement("GDRenderContextFile");

		if( pRCXElement == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a GDRenderContextFile element.\n",
				xmlFileNamePtr);
			return 0;
		}

		if( pRCXElement->GetText() == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a GDRenderContextFile element.\n",
				xmlFileNamePtr);
			return 0;
		}
		std::string rcPath;
		rcPath += "GDRenderContext/rcx_";
		rcPath += pRCXElement->GetText();
		rcPath += ".xml";

		ContentHandle<RenderContext> rcxHandle = ContentManager::LoadXML<RenderContext>( rcPath.c_str() );

		if( rcxHandle.GetContent() == 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, failed to loaf GDRenderContextFile reference.\n",
				xmlFileNamePtr);
			return 0;
		}

		TiXmlElement* pMeshElement = elementPtr->FirstChildElement("GDMeshFile");

		ContentHandle<RenderMesh> mshHandle;

		if( pMeshElement != 0 )
		{
			if( pMeshElement->GetText() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, missing a GDMeshFile element.\n",
					xmlFileNamePtr);
				return 0;
			}

			std::string meshPath = pMeshElement->GetText();


			if( strcmp( &meshPath.c_str()[ meshPath.length() - 3 ], "bin" ) != 0 )
				mshHandle = ContentManager::LoadXML<RenderMesh>( meshPath.c_str(), VertexFormatString[ rcxHandle.GetContent()->GetVertexFormat() ] );
			else
				mshHandle = ContentManager::Load<RenderMesh>( meshPath.c_str(), VertexFormatString[ rcxHandle.GetContent()->GetVertexFormat() ] );


			if( mshHandle.GetContent() == 0 )
			{
				InternalOutput::GetReference().Error(
					"Failed to load %s, failed to load GDMeshFile reference.\n",
					xmlFileNamePtr);
				return 0;
			}
		}
		
		RenderForm* resultPtr = new RenderForm;
		resultPtr->renderMeshHandle = std::move(mshHandle);
		resultPtr->renderContextHandle = std::move(rcxHandle);

		TiXmlElement* pMaterialElement = elementPtr->FirstChildElement("GDMaterialFile");
		bool bHasMaterial = false;
		if( pMaterialElement != 0 )
		{
			if( pMaterialElement->GetText() != 0 )
			{
				std::string matPath =  pMaterialElement->GetText();
				ContentHandle<RenderMaterial> materialHandle = ContentManager::LoadXML<RenderMaterial>( matPath.c_str() );

				if( materialHandle.GetContent() != 0 )
				{
					resultPtr->renderMaterialHandle = std::move(materialHandle);
					bHasMaterial = true;
				}
			}
		}

		TiXmlElement *rstElementPtr = elementPtr->FirstChildElement("RenderShapeType");
		bool isRenderShapeTypeSet = false;
		if( 0 != rstElementPtr)
		{
			string rsType = rstElementPtr->GetText();
			isRenderShapeTypeSet = true;
			// Determine the requested render shape type
			if("RenderShape" == rsType)
			{
				resultPtr->renderShapeType = RST_Basic;
			}
			else if("RenderShapeWithTexture" == rsType || "RenderShapeWithMaterial" == rsType)
			{
				resultPtr->renderShapeType = RST_WithMaterial;
			}
			else if("RenderShapeTarget" == rsType)
			{
				resultPtr->renderShapeType = RST_Target;
			}
			else if("RenderShapeGUI" == rsType)
			{
				resultPtr->renderShapeType = RST_GUI;
			}
			else if("RenderShapeDebugLines" == rsType)
			{
				resultPtr->renderShapeType = RST_DEBUG_LINES;
			}
			else if("RenderShapeLight" == rsType)
			{
				resultPtr->renderShapeType = RST_Light;
			}
			else
			{
				InternalOutput::GetReference().Error(
					"Error to loading %s, unsupported GDMaterialFile element specified, %s.\n",
					xmlFileNamePtr, rsType.c_str());
				isRenderShapeTypeSet = false;
			}
		}

		// Old file versions may not set the render shape type by xml element
		if(!isRenderShapeTypeSet)
		{
			if(!bHasMaterial)
				resultPtr->renderShapeType = RST_Basic;
			else 
				resultPtr->renderShapeType = RST_WithMaterial;
		}

		return resultPtr;
	}

	bool RenderForm::hasTexture(void)
	{
		if( renderMaterialHandle.GetContent() != 0 )
			return renderMaterialHandle.GetContent()->hasTexture();

		return false;
	}

	RenderShapeBase *RenderForm::GetNewRenderShape()
	{
		RenderShapeBase *retShape;
		switch(GetRenderShapeType())
		{
		case RST_Basic:
			retShape = new RenderShape;
			break;
		case RST_WithMaterial:
			retShape = new RenderShapeWithTexture;
			break;
		case RST_Target:
			retShape = new RenderShapeTarget;
			break;
		case RST_GUI:
			retShape = new RenderShapeGUI;
			break;
		case RST_DEBUG_LINES:
			retShape = new RenderShapeDebugLines;
			break;
		case RST_Light:
			retShape = new RenderShapeLight;
			break;
		};
		return retShape;
	}
}