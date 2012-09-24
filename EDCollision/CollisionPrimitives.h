#pragma once

#include "../EDUtilities/float3.h"
#include "CollidableTypes.h"

namespace EDCollision
{
    struct IntersectSegment : Segment
    {
        float intersectionTime;
        bool intersected;
    };
}