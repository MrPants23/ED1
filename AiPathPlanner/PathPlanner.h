//! \file PathPlanner.h
//! \author Tim Turcich, 2012; Jeremiah Blanchard, 2012
#pragma once

#include <vector>
#include "platform.h"
#include "../AiNavigationMesh/NavigationMesh.h"

namespace fullsail_ai { namespace navigation {

	class PathPlanner
	{
	public:
		//! \brief Default constructor.
		DLLEXPORT PathPlanner();

		//! \brief Destructor.
		DLLEXPORT ~PathPlanner();

		//! \brief Sets the navigation mesh.
		DLLEXPORT void initialize(const NavigationMesh& _navMesh);

		//! \brief Enters and performs the first part of the algorithm.
		DLLEXPORT void enter(const Polygon& _initial, const Polygon& _goal);

		//! \brief Returns <code>true</code> if and only if no nodes are left open.
		DLLEXPORT bool isDone() const;

		//! \brief Performs the main part of the algorithm until the specified time has elapsed or
		//! no nodes are left open.
		DLLEXPORT void update(long timeslice);

		//! \brief Returns an unmodifiable view of the solution path found by this algorithm.
		DLLEXPORT std::vector<const Polygon&> const getSolution() const;

		//! \brief Resets the algorithm.
		DLLEXPORT void exit();

		//! \brief Uninitializes the algorithm before the tile map is unloaded.
		DLLEXPORT void shutdown();
	};
}}  // namespace fullsail_ai::navigation
