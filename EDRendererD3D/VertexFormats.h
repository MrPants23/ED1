#pragma once

#pragma push_macro("new")
#undef new
#include <d3d9.h>
#include <d3dx9.h>
#pragma pop_macro("new")

namespace EDRendererD3D
{
	enum VertexFormat{ eVERTEX_POS = 0, eVERTEX_POSCOLOR, eVERTEX_POSTEX, eVERTEX_POSNORMTEX, eVERTEX_POSNORMTANTEX, eVERTEX_MAX };
	extern const char* VertexFormatString[eVERTEX_MAX];

	/// A vertex structure used to represent vertices containing only positional data.
	struct VERTEX_POS
	{
		D3DXVECTOR3	position;  // 12
	};

	/// A vertex structure used to represent vertices containing position and color data.
	struct VERTEX_POSCOLOR
	{
		D3DXVECTOR3	position;  // 12
		D3DCOLOR	color;	   // 4
	};

	/// A vertex structure used to represent vertices containing position and texture coordinate data.
	struct VERTEX_POSTEX
	{
		D3DXVECTOR3	position;  // 12
		D3DXVECTOR2	texcoord;  // 8
	};

	/// A vertex structure used to represent vertices containing position, normal and texture coordinate data.
	struct VERTEX_POSNORMTEX
	{
		D3DXVECTOR3	position;  // 12
		D3DXVECTOR3	normal;  // 12
		D3DXVECTOR2	texcoord;  // 8
	};

	/// A vertex structure used to represent vertices containing position, normal and texture coordinate data.
	struct VERTEX_POSNORMTANTEX
	{
		D3DXVECTOR3	position;  // 12
		D3DXVECTOR3	normal;  // 12
		D3DXVECTOR4 tangent; // 16
		D3DXVECTOR2	texcoord;  // 8
	};
}