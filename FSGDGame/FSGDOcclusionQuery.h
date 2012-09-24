#include "../EDGOCA/DynAabbTreeNode.h"
using namespace EDGOCA;

#include "RenderController.h"

class FSGDOcclusionQuery : public IOcclusionQueryInterface
{
	VisibilityQueryInterface visQueryInterface;

public:
	
	void Initialize(void);
	void Issue(const Aabb& aabb);
	int GetQueryResult(void);
	~FSGDOcclusionQuery();

	static IOcclusionQueryInterface* creator(void);
};