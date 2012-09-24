#pragma once
#include "../tinyxml/tinyxml.h"
#include "float4x4.h"
using namespace EDUtilities;

void ReadXmlVector4( TiXmlElement* axisElementPtr, float* vectorPtr );

void ReadXmlMatrix( TiXmlElement* matrixElementPtr, float4x4 *matrixPtr );