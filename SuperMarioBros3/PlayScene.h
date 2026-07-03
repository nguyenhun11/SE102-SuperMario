#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "ParaGoomba.h"
#include "HUD.h"
#include "Switch.h"
#include "GameManager.h"

// Forward declaration

#define SWITCH_ACTIVATION_TIME 10000 // thời gian hiệu lực của switch
#define CAMERA_TRANSITION_TIME 500

struct CameraZone
{
	float left, top, right, bottom;
};


class PlayScene: public Scene
{
private: 
	SwitchType currentSwitchType = SwitchType::BrickToCoin;

	bool isAutoScroll = false;
	float autoScrollSpeed = 0.0f;
protected: 
	// A play scene has to have player, right? 
	LPGAMEOBJECT player;					

	vector<LPGAMEOBJECT> objects;
	vector<CameraZone> cameraZones;

	void _ParseSection_MAP_INFO(string line);
	void _ParseSection_CAMERA_ZONES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_BGM(string line);

	void _ParseSection_ASSETS(string line);
	void _ParseSection_OBJECTS(string line, bool isGridCoordinat = false);

	void LoadAssets(LPCWSTR assetFile);

	float mapRight = 200;	// này tính theo tile (16 x 16)
	float mapLeft = -0.5f;
	float mapTop = -500;    
	float mapBottom = 15;

	float startCamY = 0.0f;
	float currentCamY = 0.0f;
	CameraZone defaultCameraZone = { 0,0,0,0 };
	CameraZone prevCameraZone = defaultCameraZone;
	float cameraTransitionTimer = 0.0f;
	bool isTransitioningCamera = false;

	
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
	CameraZone GetCurrentZone(float x, float y);

	static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);

	void ActivatePSwitch(bool isFromLoad);
	void DeactivatePSwitch();
};

typedef PlayScene* LPPLAYSCENE;

