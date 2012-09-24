#pragma once

#include "../EDCollision/CollidableTypes.h"
using namespace EDCollision;

#include <set>
using namespace std;

#include "DynAabbTreeNode.h"
#include "MessageT.h"

namespace EDGOCA
{
	class IMessage;

	class DynAabbTree
	{
		static unsigned int frameID;

		DynAabbTreeNode* root;

		DynAabbTreeNode* Insert(DynAabbTreeNode* insertPoint, DynAabbTreeLeafNode* insertion);

		static float Cost(DynAabbTreeNode* first, DynAabbTreeNode* second);

		set<DynAabbTreeLeafNode*> leafList;

		static void TreeCollision(DynAabbTreeNode* nodeA, DynAabbTreeNode* nodeB);
		static void SelfTreeCollision(DynAabbTreeLeafNode* leafPtr, DynAabbTreeNode* node);

		IOcclusionQueryCreator occlusionQueryCreator;

		static void FrustumAndOcclusionCull(DynAabbTreeNode *node, DynAabbTreeNode*& queryQueueHead, DynAabbTreeNode*& queryQueueTail, FrustumHull* frustum, IMessage* msg);
		static void FrustumCull(DynAabbTreeNode* node, FrustumHull* frustum, IMessage* msg);

	public:
		DynAabbTree(void) : root(0), occlusionQueryCreator(0) {}

		~DynAabbTree(void);
	
		DynAabbTreeNode* Insert(DynAabbTreeLeafNode* insertion);

		DynAabbTreeNode* Remove(DynAabbTreeLeafNode* leaf);

		void Update(DynAabbTreeLeafNode* leaf, const Aabb& newBounds);

		static void TreeCollision(DynAabbTree* treeA, DynAabbTree* treeB);

		static void FrustumCull(DynAabbTree* tree, FrustumHull* frustum, IMessage* msg);
		static void FrustumAndOcclusionCull(DynAabbTree* tree, FrustumHull* frustum, IMessage* msg);

		void SetOcclusionQueryCreator(IOcclusionQueryCreator creator);
		IOcclusionQueryCreator GetOcclusionQueryCreator(void){ return occlusionQueryCreator; }
	};
}