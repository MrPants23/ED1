#pragma once
#include "RenderController.h"

class PauseEffect
{
public:
	PauseEffect(void);
	~PauseEffect(void);

	void Initialize();
	void Activate();
	void Deactivate();

	static void PauseEffectUpdate(unsigned int eventId, void *eventData, void *listener);
private:
	PostProcess pauseFadePostProcess;
	float theta;
	float scale;
	float maxScale;
	float fade;
	float maxFade;
	D3DXHANDLE thetaHandle;
	D3DXHANDLE scaleHandle;
	D3DXHANDLE fadeHandle;
};


