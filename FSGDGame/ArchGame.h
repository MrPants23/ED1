#pragma once

#include "GameBase.h"
#include "../EDRendererD3D/RenderSprite.h"
#include "PauseEffect.h"

#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/Scene.h"
using namespace EDGOCA;

/// The main game class.
class ArchGame : public GameBase
{
private:
	Scene m_TestScene;

	vector<GameObject *> cameras;

	/// pause state boolean
	bool isPaused;

	/// An effect to be run while paused
	PauseEffect pauseEffect;

	/// GUI element to show while paused
	EDRendererD3D::RenderSprite pauseSprite;

	/// Sprite to represent background of the print console
	EDRendererD3D::RenderSprite consoleBackground;

	/// Initializes test entities and their components.
	void InitTestObjects();

public:
	ArchGame(void);
	/// Deletes test objects and singleton instances that should no longer be used.
	~ArchGame(void);

	/// Method for setting the camera projection and frustum shape
	void Resize(int width, int height);
	/// Initializes the main game data.
	void Initialize();
	/// Updates main game objects.  Currently adds RenderShapes to batches for rendering.  
	/// This will be done by the spatial system in the near future.
	void Update();
	/// Renders the RenderController, PrintConsole and InputConsole and swaps buffers.
	void Render();

	void TogglePause();

	/// Method used to switch controls from one entity to another
	void ToggleActiveController();

	void ToggleActiveViewer();

	void ExitSplashScreen();

private:

	/*Navigation System DLL*/

	/*Forward Decs*/
	friend class AINavigation;
	friend class AINavModule;
	friend class NavDebugRender;

	/*Actual Systems & Modules*/
	AINavigation* myNavSystem;
	NavDebugRender* myNavDebugRender;
	AINavModule* tempAStar;
};

