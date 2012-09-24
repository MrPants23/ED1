#include "RenderMaterial.h"
#define WIN32_LEAN_AND_MEAN
#include "../EDMemoryManager/MMAllocator.h"
#include "../tinyxml/tinyxml.h"
#include "Renderer.h"
#include "../EDUtilities/InternalOutput.h"

namespace EDRendererD3D
{
	bool RenderMaterial::hasTexture(void)
	{
		if( renderTextureCubeMapHandle.GetContent() != 0 )
			if( renderTextureCubeMapHandle.GetContent()->GetHandle() != 0 )
				return true;

		for(unsigned int i = 0; i < renderTextureHandles.size(); ++i)
			if( renderTextureHandles[i].GetContent() != 0 )
				if( renderTextureHandles[i].GetContent()->GetHandle() != 0 )
					return true;

		return false;
	}

	//RenderTextureCubeMap* RenderTextureCubeMap::Load(const char* fileName)
	//{
	//	InternalOutput::GetReference() << "Loading " << fileName << "...\n";

	//	WCHAR *buffer = new WCHAR[strlen(fileName)+1];

	//	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fileName, 
	//		strlen(fileName), buffer, strlen(fileName));

	//	buffer[strlen(fileName)] = 0;

	//	RenderTextureCubeMap* pRenderTextureCubeMap = new RenderTextureCubeMap;

	//	D3DXCreateCubeTextureFromFile(Renderer::theDevicePtr, buffer, (IDirect3DCubeTexture9 **)&pRenderTextureCubeMap->textureHandle);
 //   
	//	delete[] buffer;

	//	return pRenderTextureCubeMap;	
	//}

	RenderMaterial* RenderMaterial::LoadXML(const char* xmlFileName, const char* indentifier)
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

		if( strcmp(rootName.c_str(), "GDMaterial") != 0 )
		{
			InternalOutput::GetReference().Error(
				"Failed to load %s, missing a GDMaterial element\n", 
				xmlFileNamePtr);
			return 0;
		}

		RenderMaterial* pRenderMaterial = new RenderMaterial;

		hRoot = TiXmlHandle(elementPtr);

		const char* propertyString[] = { "Ambient", "Diffuse", "Specular", "Emmisive", "Reflected" };
		const char* propertyMapString[] = {"Ambient_Map", "Diffuse_Map", "Specular_Map", "Emmisive_Map", "Normal_Map"};
		const char* pColorComponents[] = { "r", "g", "b", "a" };
		for(unsigned int i = 0; i < 5; ++i)
		{
			RenderColor propertyColor;
			propertyColor.Red = propertyColor.Green = propertyColor.Blue = propertyColor.Alpha = 1.0f;

			TiXmlElement* pPropertyElement = elementPtr->FirstChildElement(propertyString[i]);
			
			if( pPropertyElement != 0 )
			{
				for(unsigned int comp = 0; comp < 4; ++comp)
				{
					TiXmlElement* pComponentElement = pPropertyElement->FirstChildElement(pColorComponents[comp]);
					if( pComponentElement != 0 )
						propertyColor.RGBA[comp] = (float)atof( pComponentElement->GetText() );
				}
			}
			pRenderMaterial->renderColors.push_back(propertyColor);

			ContentHandle<RenderTexture> rtHandle;
			TiXmlElement* pPropertyMapElement = elementPtr->FirstChildElement(propertyMapString[i]);

			if( pPropertyMapElement != 0 )
			{
				std::string filePath = "Textures/";
				filePath += pPropertyMapElement->GetText();

				rtHandle = ContentManager::Load<RenderTexture>( filePath.c_str() );
			}

			pRenderMaterial->renderTextureHandles.push_back(rtHandle);
		}

		ContentHandle<RenderTextureCubeMap> rtHandle;
		TiXmlElement* pPropertyMapElement = elementPtr->FirstChildElement("Reflected_Map");
		if( pPropertyMapElement != 0 )
		{
			std::string filePath = "Textures/";
			filePath += pPropertyMapElement->GetText();

			rtHandle = ContentManager::Load<RenderTextureCubeMap>( filePath.c_str() );	
		}

		pRenderMaterial->renderTextureCubeMapHandle = std::move(rtHandle);

		return pRenderMaterial;
	}
}