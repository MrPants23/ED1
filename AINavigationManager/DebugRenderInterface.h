#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

#include "AIDef.h"

class AINavModule;

class DebugRenderInterface
{
public:
	virtual void setDevice(LPDIRECT3DDEVICE9 lpDevice) = 0;
	virtual bool SetPolygonColor(AINavModule* releventModule, unsigned int polyIndex, float red, float green, float blue, float alpha) = 0;
	virtual bool SetPolygonEdgeColor(AINavModule* releventModule, unsigned int polyIndex, float red, float green, float blue, float alpha) = 0;
	virtual void RenderNavMesh(AINavModule* releventModule,const AIMatrix44& proj,const AIMatrix44& view) = 0;
};

