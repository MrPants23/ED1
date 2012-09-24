#pragma once
#include "RenderShape.h"

#include <vector>
using std::vector;

namespace EDRendererD3D
{
	class RenderShapeDebugLines : public RenderShape
	{
	public:
		RenderShapeDebugLines(void);
		~RenderShapeDebugLines(void);

		void AddLines(const VERTEX_POSCOLOR *_points, UINT primCount);
		void ClearLines(void);
		inline void SetEffect(ID3DXEffect *_effect) { effect = _effect; }

		static void RenderDebugLines(RenderNode &node);

		//Frame *GetFramePtr() { return 0; }
		//RenderContext *GetRenderContext(int lod = -1) { return 0; }
		//void AddToContextSet()  { };
	private:
		vector<VERTEX_POSCOLOR> points;
		ID3DXEffect *effect;
	};
}