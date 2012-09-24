//! \file Tile.h
//! \brief Defines the <code>fullsail_ai::navigation::Polygon</code> class.
#pragma once

#include "../EDUtilities/float3.h"
typedef EDUtilities::float3 Point;

#include <vector>
using namespace std;

namespace fullsail_ai { namespace navigation {

	struct Edge
	{
		float cost;
		Polygon& endpoint;
	};

	//! \brief Logical representation of a polygon in a navigation mesh.
	class Polygon
	{
		friend class NavigationMesh;

		vector<Point&> vertices;
		vector<Edge&> connections;

		// For drawing purposes
		unsigned int marker_color, outline_color, fill_color, line_color;
		Polygon* line_destination;
		void resetDrawing();

	public:

		//! \brief Returns the LRGB marker color of this tile.
		unsigned int getMarker() const;
		unsigned int getOutline() const;
		unsigned int getFill() const;
		const Polygon& getLineDestination() const;
		unsigned int getLine() const;

		//! \brief Sets this tile's market color to the designated color in the LRGB color space.
		void setMarker(unsigned int color);
		void setOutline(unsigned int color);
		void setFill(unsigned int color);
		void setLineTo(Polygon& destination, unsigned int color);
	};
}}  // namespace fullsail_ai
