#pragma once

#include "../EDRendererD3D/RenderShapeWithTexture.h"
using EDRendererD3D::RenderShapeWithTexture;

namespace EDRendererD3D
{
    template<typename RenderShapeType>
    RenderModel<RenderShapeType>::RenderModel(const char* fileName)
    {
        Read(fileName);
    }
}