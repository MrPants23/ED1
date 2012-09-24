#pragma once

#include "EDTree.h"
#include "../EDCollision/CollisionLibrary.h"
using namespace EDCollision;

#include "../EDUtilities/float3.h"
using namespace EDUtilities;

struct SourceData
{
	const EDUtilities::float3* pointsPtr;
	EDUtilities::float3* centroidsPtr;
	const unsigned int* indiciesPtr;
	unsigned int maxLeafSize;
};

class TiXmlElement;

class AabbBvh : public EDTree
{
protected:
	void* sourceDataPtr;
	//unsigned int* m_ReorderedIndices;
	//unsigned int m_IndexCount;

public:
	AabbBvh(void){}
	AabbBvh(const EDUtilities::float3 *pointsPtr, const unsigned int *indicesPtr, unsigned int numIndices, unsigned int maxLeafSize);
	const void* GetSourceData(void)const{ return sourceDataPtr; }
	virtual ~AabbBvh(){}
	void BuildTree(const EDUtilities::float3* pointsPtr, const unsigned int* indicesPtr, unsigned int numIndices, unsigned int maxLeafSize);
	static AabbBvh* LoadXML(const char* fileName);
	static AabbBvh* LoadXML(TiXmlElement* elementPtr);

	void GetLocalAabb(Aabb& aabb) const;
};