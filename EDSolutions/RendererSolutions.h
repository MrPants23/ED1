#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

namespace EDRendererD3D
{
    class RenderSet;
    
    void InitializeSolution(HWND hWnd);
    void InitializeSolution(D3DPRESENT_PARAMETERS &d3dpp);
    void RenderSolution(RenderSet &set);
}
