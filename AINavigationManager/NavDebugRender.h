#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

//#pragma comment(lib, "d3d9.lib")
//#pragma comment(lib, "d3dx9.lib")
//#pragma comment (lib, "dxguid.lib")
#include "AIDef.h"
#include "DebugRenderInterface.h"

#include <map>

struct NavPolygon;
class AINavigation;
class AINavModule;

/*All this class does is hook with the AINavigation system and renders the navigation mesh of and according to how any one single module is using it to navaigate*/
/*Toggling which module is being rendered is included*/
class NavDebugRender : public DebugRenderInterface
{
	/*This Poly info will exist in an array 1 to 1 with the polgyons in the navigation mesh*/
	struct NavPolyDrawInfo
	{
		//unsigned int vertex_offset_from_mesh_vertex_offset;
		float Color[4];
		float Outline_Color[4];
		NavPolyDrawInfo()
		{
			Color[0] = 0.7f;
			Color[1] = 0.7f;
			Color[2] = 0.7f;
			Color[3] = 0.2f;

			Outline_Color[0] = 1.0f;
			Outline_Color[1] = 1.0f;
			Outline_Color[2] = 1.0f;
			Outline_Color[3] = 1.0f;
		}

	};

	/*This Mesh info will exist for each entity in the system associated with this debug renderer*/
	struct MeshDrawInfo
	{
		unsigned int size_of_verts;
		unsigned int offset_in_vert_buffer;
		unsigned int vertex_offset;
		unsigned int outline_offset;
		NavPolyDrawInfo* polyInfo;
		MeshDrawInfo()
		{
			size_of_verts = 0;
			offset_in_vert_buffer = 0;
			polyInfo = 0;
			vertex_offset = 0;
		}

		void Init(unsigned int numPolies)
		{
			polyInfo = new NavPolyDrawInfo[numPolies];
		}

		~MeshDrawInfo(void)
		{
			if(polyInfo)
			{
				delete polyInfo;
				polyInfo = 0;
			}
		}
	};

	struct DefaultRenderVertex
	{
		AIVector3 position;
		//D3DXVECTOR3 normal;
		//D3DXVECTOR2 tex;
		//D3DCOLOR color;
	};

	AINavigation* navSystem;
	std::map<AINavModule*, MeshDrawInfo*> meshInfo;

	unsigned int activeModuleIndex;

	LPDIRECT3DDEVICE9				mylpDevice;

	/*Effect Files*/
	static ID3DXEffect*					myEffectFile;

	/*Transform Handles*/
	D3DXHANDLE						myWorldViewProj;
	D3DXHANDLE						myWorldView;
	D3DXHANDLE						myWorld;
	D3DXHANDLE						myView;
	D3DXHANDLE						myProj;

	/*D3DXHANDLE						myCameraPos;*/

	/*Technique Handles*/
	D3DXHANDLE						myPolyColorTechnique;
	D3DXHANDLE						myLinesTechnique;
	//D3DXHANDLE						myWireTechnique;

	///*PointLight Handles*/
	//D3DXHANDLE						myPointLightPos;
	//D3DXHANDLE						myPointLightRange;
	//D3DXHANDLE						myPointLightColor;
	//D3DXHANDLE						myPointLightSpecColor;
	//D3DXHANDLE						myPointLightSpecPower;

	/*Other handles*/
	D3DXHANDLE						myColour;


	static D3DVERTEXELEMENT9				myDefaultVertexElements[];
	static IDirect3DVertexDeclaration9*		myDefaultVertexDeclaration;
	static IDirect3DVertexBuffer9*			myDefaultVertexBuff;
	static IDirect3DIndexBuffer9*			myDefaultIndexBuff;

	//This function is called in inside RenderNavMesh... What this does is checks to see if the number of entities in the navigation system change.  
	//If they have it Updates the index and vertex buffer with the new information.
	void UpdateBuffersIfNeeded(void);

public:
	NavDebugRender(AINavigation* navsystem, LPDIRECT3DDEVICE9 lpDevice = 0);
	~NavDebugRender(void);

	void setDevice(LPDIRECT3DDEVICE9 lpDevice);

	bool SetPolygonColor(AINavModule* releventModule, unsigned int polyIndex, float red, float green, float blue, float alpha);
	bool SetPolygonEdgeColor(AINavModule* releventModule, unsigned int polyIndex, float red, float green, float blue, float alpha);
	void RenderNavMesh(AINavModule* releventModule,const AIMatrix44& proj,const AIMatrix44& view);
};