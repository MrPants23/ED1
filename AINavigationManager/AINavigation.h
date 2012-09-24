#ifndef AI_NAVIGATION
#define AI_NAVIGATION
/*AI Navigation*/
/*Tim Turcich*/
/*Some point in comprehension*/
#include <vector>
#include <map>
#include "AIDef.h"
#include "NavInterface.h"
#include "NavMesh.h"

class AINavModule;
class NavMesh;

class AINavigation : public NavInterface
{
public:
	friend class NavDebugRender;
	/*Private Interface*/
	AINavigation();
	AINavigation(const AINavigation& ref){}
	AINavigation& operator=(const AINavigation& ref){}
	~AINavigation();
	/*Public Interfaces*/

private:
	std::vector<AINavModule*> myModules;
	//std::vector<NavMesh*> myMeshes;//I might use this to hold all active meshes in the system...right now the load returns the mesh to be used with the AddModule Function

public:
	//enum PATHFINDING_TYPE { ASTAR };

	static AINavigation& GetReference();

	void ProcessNaviagtion();
	void SyncNaviation(unsigned int wait_millsecs);

	/*add a new navigation module of the given enum type and return the memory by pointer*/
	AINavModule* AddModule(PATHFINDING_TYPE newPathModuleType, NavMesh* mesh_to_navigate);
	void SetNavModulesPath(AINavModule* module, AIVector3& wherefrom, AIVector3& whereto);
	void RemoveModule(AINavModule* module);

	NavMesh* LoadNavMesh(const char* filename);
};


#endif