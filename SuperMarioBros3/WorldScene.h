#pragma once

#include <string>
#include "Scene.h"
#include "GameObject.h"
#include "Textures.h"

class WorldScene : public Scene
{
protected:
	LPGAMEOBJECT worldPlayer;
	vector<LPGAMEOBJECT> objects;

	void _ParseSection_ASSETS(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_OBJECTS(string line);

	void LoadAssets(LPCWSTR assetFile);

public:
	WorldScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	LPGAMEOBJECT GetPlayer() { return worldPlayer; }
	void AddObject(LPGAMEOBJECT obj) { objects.push_back(obj); }
	void Clear();
	void PurgeDeletedObjects();
};

typedef WorldScene* LPWORLDSCENE;