#ifndef AI_NAV_MODULE
#define AI_NAV_MODULE
/*AINavModule*/
/*Tim Turcich*/
/*A little later*/
#include "AIDef.h"

class AINavModule
{
	friend class NavMesh;
	friend class AINavigation;
	friend class NavDebugRender;
	unsigned int ID;//system handle used by AINavigation Class

	static volatile unsigned int refCount;

	AIVector3 myStartPosition;//start position for navigation
	AIVector3 myEndPosition;//end position for navigation

	/*Thread Related Stuff*/
	void setDesiredPath(AIVector3& myStartPos, AIVector3& myEndPos) { myStartPosition = myStartPos; myEndPosition = myEndPos; resetpath = true; }

public:
	AINavModule();
	virtual ~AINavModule() = 0;

	//Interface for anyone implementing path finding to use
	virtual void Init(NavMesh* mesh_to_navigate) {}//Standing Memory
	virtual void Enter(void) {}//Activity Memory
	virtual void Update(void) {}//Activity Process
	virtual void Exit(void) {}//Activity Clean
	virtual void Shutdown(void) {}//Clean ALL

protected:
	volatile bool thread_running;//controlling the thread as a whole
	volatile bool isFinished;//controlling one iteration of the algorythm 
	volatile bool resetpath;//controlling restart of algorythm based on input changes
	
	HANDLE hThread;

	NavMesh* myNavMesh;//mesh that this module currently cares about
};
#endif