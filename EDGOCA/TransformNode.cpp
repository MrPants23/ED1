#include "TransformNode.h"
#include <assert.h>


namespace EDGOCA
{
	const float4x4& TransformNode::GetWorldTransform(void)
	{
		// Put in your own solution for this method for the Sorting and hierarchy lab
		return GetWorldTransformSolution();
	}

	void TransformNode::DirtyNode(void)
	{
		// Put in your own solution for this method for the Sorting and hierarchy lab
		return DirtyNodeSolution();
	}

	TransformNode* TransformNode::GetRoot(void)
	{
		if( parentPtr == 0 )
			return this;

		return parentPtr->GetRoot();
	}

	TransformNode::~TransformNode()
	{
		while( !children.empty() )
			RemoveChild( (*children.begin()) );
	}

	void TransformNode::SetLocalTransform(const float4x4& transform)
	{
		localTransform = transform;
		DirtyNode();
	}

	void TransformNode::SetLocalPosition(const float3& position)
	{
		localTransform.WAxis = position;
		DirtyNode();
	}

	void TransformNode::TranslateLocal(const float3& delta)
	{
		localTransform.translateLocal(delta);
		DirtyNode();
	}

	void TransformNode::TranslateGlobal(const float3& delta)
	{
		localTransform.translateGlobal(delta);
		DirtyNode();
	}
		
	void TransformNode::RotateLocalX(float radians)
	{
		localTransform.rotateLocalX(radians);
		DirtyNode();
	}

	void TransformNode::RotateLocalY(float radians)
	{
		localTransform.rotateLocalY(radians);
		DirtyNode();
	}

	void TransformNode::RotateLocalZ(float radians)
	{
		localTransform.rotateLocalZ(radians);
		DirtyNode();
	}

	void TransformNode::RotateGlobalX(float radians)
	{
		localTransform.rotateGlobalX(radians);
		DirtyNode();
	}

	void TransformNode::RotateGlobalY(float radians)
	{
		localTransform.rotateGlobalY(radians);
		DirtyNode();
	}

	void TransformNode::RotateGlobalZ(float radians)
	{
		localTransform.rotateGlobalZ(radians);
		DirtyNode();
	}
	
	void TransformNode::AddChild(TransformNode* childNodePtr)
	{
		assert( childNodePtr != 0 );

		if( childNodePtr->parentPtr != this )
		{
			if( childNodePtr->parentPtr != 0 )
				childNodePtr->parentPtr->RemoveChild(childNodePtr);

			childNodePtr->parentPtr = this;
			children.insert( childNodePtr );
			childNodePtr->DirtyNode();	
		}
	}

	void TransformNode::RemoveChild(TransformNode* childNodePtr)
	{
		if( childNodePtr->parentPtr == this )
		{
			children.erase( childNodePtr );
			childNodePtr->parentPtr = 0;
			childNodePtr->DirtyNode();
		}
	}
}