//! \file Tile.h
//! \brief Defines the <code>fullsail_ai::navigation::NavigationMesh</code> class.
#pragma once

#include <vector>
#include "Polygon.h"
#include "platform.h"

using namespace std;

namespace fullsail_ai { namespace navigation {

	//! \brief Logical representation of a navigation mesh.
	class NavigationMesh
	{
		vector<Point&> vertices;
		vector<Polygon&> polygons;

	public:
		//! \brief Constructs a new <code>%NavigationMesh</code> object.
		DLLEXPORT NavigationMesh();

		//! \brief Copy constructor.
		DLLEXPORT NavigationMesh(const NavigationMesh&);

		//! \brief Assignment operator.
		DLLEXPORT NavigationMesh& operator=(const NavigationMesh&);

		//! \brief Destroys this <code>%NavigationMesh</code> object.
		DLLEXPORT ~NavigationMesh();

		//! \brief Loads data stored into a file into memory in this <code>%NavigationMesh</code> object.
		bool LoadBin(const char* filename);

		//! \brief Resets all drawing colors set in the tiles to transparent black (0x00000000).
		DLLEXPORT void resetMeshDrawing();
	};
}}  // namespace fullsail_ai::navigation


