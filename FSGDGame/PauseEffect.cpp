#include "PauseEffect.h"
#include "WorldData.h"
#include "../EDEventManager/EventManager.h"

PauseEffect::PauseEffect(void)
{
	theta = 0;
	maxScale = 5;
	maxFade = 0.9f;
}

PauseEffect::~PauseEffect(void)
{
}

void PauseEffect::Initialize()
{
	pauseFadePostProcess.Initialize(RenderController::GetInstance()->GetScreenQuadPtr(), 
		L"Fade.fx");
	thetaHandle = pauseFadePostProcess.GetEffect()->GetParameterByName(0, "gTheta");
	scaleHandle = pauseFadePostProcess.GetEffect()->GetParameterByName(0, "gScale");
	fadeHandle = pauseFadePostProcess.GetEffect()->GetParameterByName(0, "gFade");

	EDEventManager::EventManager::GetInstance()->RegisterForEvent(
		"PauseEffectUpdate", this, PauseEffectUpdate);
}

void PauseEffect::Activate()
{
	RenderController::GetInstance()->PushBackPostProcess(
		&pauseFadePostProcess);

	EDEventManager::EventManager::GetInstance()->PostEvent("PauseEffectUpdate", 0, 
		EDEventManager::EM_PRIORITY_NORMAL, EDEventManager::EM_FLAG_PERSISTENT);

	scale = 0;
	fade = 1.0f;
	theta = 0;
}

void PauseEffect::Deactivate()
{
	RenderController::GetInstance()->RemovePostProcess(
		&pauseFadePostProcess);

	EDEventManager::EventManager::GetInstance()->DeletePersistentEvent("PauseEffectUpdate", 0);
}

void PauseEffect::PauseEffectUpdate(unsigned int eventId, void *eventData, void *listener)
{
	PauseEffect *pauseEffect = (PauseEffect *)listener;
	pauseEffect->pauseFadePostProcess.GetEffect()->SetFloat(
		pauseEffect->thetaHandle, pauseEffect->theta += 
		(WorldData::GetDeltaTime() * pauseEffect->scale / pauseEffect->maxScale));

	if(pauseEffect->maxScale > pauseEffect->scale)
	{
		pauseEffect->pauseFadePostProcess.GetEffect()->SetFloat(
			pauseEffect->scaleHandle, pauseEffect->scale += WorldData::GetDeltaTime());

		pauseEffect->fade = 1.0f - ( pauseEffect->maxFade * (pauseEffect->scale / pauseEffect->maxScale));
		pauseEffect->pauseFadePostProcess.GetEffect()->SetFloat(
			pauseEffect->fadeHandle, pauseEffect->fade);
	}

}
