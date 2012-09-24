#pragma once
#include "AIDef.h"

class AINavModule;
class NavMesh;

class NavInterface
{
public:
	enum PATHFINDING_TYPE { ASTAR };
	/*NavInterface(void);
	virtual ~NavInterface(void) = 0;*/
	virtual void ProcessNaviagtion() = 0;
	virtual void SyncNaviation(unsigned int wait_millsecs) = 0;
	virtual AINavModule* AddModule(PATHFINDING_TYPE newPathModuleType, NavMesh* mesh_to_navigate) = 0;
	virtual void SetNavModulesPath(AINavModule* module, AIVector3& wherefrom, AIVector3& whereto) = 0;
	virtual void RemoveModule(AINavModule* module) = 0;
	virtual NavMesh* LoadNavMesh(const char* filename) = 0;
};

