#pragma once

#include <vector>
//#include "AIDef.h"

struct NavPolygon
{
	unsigned int dimension;


	unsigned int* vertIndicies;//Right now i'm doing triangles
	int* adjacentPolygons;//the connecting polygons...up to 3 since they are triangles right now
	//[0] polygon shares the edge of [0]-[1] verts
	//[1] polygon shares the edge of [1]-[2] verts
	//[2] polygon shares the edge of [2]-[0] verts
	//if there is no connection along a side then the pointer will be 0
};

/*Represents a class of interconnecting triangles and or polygons*/
class NavMesh
{
	friend class AINavigation;
	friend class NavDebugRender;

	NavMesh();
	~NavMesh();
	bool LoadBin(const char* filename);//for loading direct nav mesh file all read prepared

	unsigned int numVerts;
	AIVector3* meshVerts;
	unsigned int numPolygons;
	NavPolygon* meshPolygons;
};