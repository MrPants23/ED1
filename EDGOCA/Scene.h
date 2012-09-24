#pragma once

#include "DynAabbTree.h"
#include "GameObject.h"
#include "DynAabbTreeNode.h"

#include <map>
#include <list>
using namespace std;

namespace EDGOCA
{
	class Scene
	{
		DynAabbTree dynamicObjectTree;
		DynAabbTree staticObjectTree;

		DynAabbTree visTree;

		list<GameObject*> updateables;
		map<unsigned int, GameObject*> uniques;

		list<GameObject*> deletingObjects;

		set<GameObject*> masterObjectSet;

		unsigned int sceneFlags;

	public:

		enum SceneFlag{ SF_OCCLUSION_CULL = 1 }; 

		Scene(void) : sceneFlags(0) {}
		~Scene();

		GameObject* SpawnObject(const char* xmlFileNamePtr, const float4x4* xform = 0);
		void DespawnObject(GameObject* object);

		GameObject* GetUniqueObject(unsigned int id);

		void Update(void);

		void FrustumCull( FrustumHull* frustum, IMessage* msg );

		void SetObjectRenderableState(GameObject* object, bool state);

		void SetOcclusionQueryCreator(IOcclusionQueryCreator creator);

		unsigned int GetSceneFlags(void){ return sceneFlags; }
		void EnableSceneFlags( unsigned int flags ){ sceneFlags |= flags; }
		void DisableSceneFlags( unsigned int flags ){ sceneFlags &= ~flags; }
	};
}