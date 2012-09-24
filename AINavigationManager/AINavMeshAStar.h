#ifndef AI_NAV_MESH_ASTAR
#define AI_NAV_MESH_ASTAR
#include "AINavModule.h"

class AINavMeshAStar : public AINavModule
{
	friend class AINavigation;
	AINavMeshAStar();
	~AINavMeshAStar();
	void Init(NavMesh* mesh_to_navigate);
	void Enter();
	void Update();
	void Exit();
	void Shutdown();
	static unsigned int _stdcall AStarNavProc(void* params);
};
#endif