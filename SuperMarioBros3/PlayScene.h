#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "HUD.h"
#include "GameManager.h"
//#include "Koopas.h"


class PlayScene: public Scene
{
protected: 
	// A play scene has to have player, right? 
	LPGAMEOBJECT player;					

	vector<LPGAMEOBJECT> objects;

	void _ParseSection_MAP_INFO(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);

	void _ParseSection_ASSETS(string line);
	void _ParseSection_OBJECTS(string line, bool isGridCoordinat = false);

	void LoadAssets(LPCWSTR assetFile);

	float mapRight = 175;	// này tính theo tile (16 x 16)
	// 3 cái dưới này tính theo pixel
	float mapLeft = -8.0f;
	float mapTop = -300.0f;    
	float mapBottom = 240.0f;
	
public: 
	PlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	LPGAMEOBJECT GetPlayer() { return player; }
	void AddObject(LPGAMEOBJECT obj) { objects.push_back(obj); }
	void InsertObjectToFront(LPGAMEOBJECT obj)
	{
		objects.insert(objects.begin(), obj);
	}

	void Clear();
	void PurgeDeletedObjects();

	static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef PlayScene* LPPLAYSCENE;

