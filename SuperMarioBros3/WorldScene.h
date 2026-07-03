#pragma once

#include <string>
#include "Scene.h"
#include "GameObject.h"
#include "Textures.h"
#include "MapBackground.h"

#define TILE_SIZE 16.0f

class WorldScene : public Scene
{
protected:
	LPGAMEOBJECT worldMario;

	float mapWidth = 0.0f;
	float mapHeight = 0.0f;
	vector<LPGAMEOBJECT> objects;

	void _ParseSection_ASSETS(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_OBJECTS(string line, bool isGridCoordinate = false);
	void _ParseSection_BGM(string line);

	void LoadAssets(LPCWSTR assetFile);


public:
	WorldScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	LPGAMEOBJECT GetPlayer() { return worldMario; }
	void AddObject(LPGAMEOBJECT obj) { objects.push_back(obj); }
	void Clear();
	void PurgeDeletedObjects();
};

typedef WorldScene* LPWORLDSCENE;