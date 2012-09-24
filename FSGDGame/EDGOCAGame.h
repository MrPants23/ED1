#pragma once

#include "GameBase.h"

#include "../EDGOCA/GameObject.h"
#include "../EDGOCA/Scene.h"
using namespace EDGOCA;

/// The main game class.
class EDGOCAGame : public GameBase
{
private:

	/// Initializes test entities and their components.
	void InitTestObjects();

	Scene m_TestScene;

	GameObject* m_TestObject;

	vector<GameObject *> cameras;


public:
	EDGOCAGame(void);
	/// Deletes test objects and singleton instances that should no longer be used.
	~EDGOCAGame(void);

	/// Method for setting the camera projection and frustum shape
	void Resize(int width, int height);
	/// Initializes the main game data.
	void Initialize();
	/// Updates main game objects.  Currently adds RenderShapes to batches for rendering.  
	/// This will be done by the spatial system in the near future.
	void Update();
	/// Renders the RenderController, PrintConsole and InputConsole and swaps buffers.
	void Render();
};