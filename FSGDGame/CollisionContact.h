#pragma once
#include "../EDUtilities/float3.h"

using namespace EDUtilities;

struct CollisionContact
{
	float3 m_ContactPoint;
	float3 m_ContactNormal;
	float m_PenetrationDepth;
};
