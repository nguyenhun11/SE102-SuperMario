#include <iostream>
#include <fstream>
#include "AssetIDs.h"
#include "debug.h"

#include "GameGlobal.h"
#include "WorldSceneKeyHandler.h"
#include "WorldScene.h"
#include "HUD.h"
#include "GameManager.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Camera.h"
#include "MapBackground.h"
#include "MapMario.h"
#include "WorldMapObject.h"
#include "InvisibleNode.h"
#include "StageNode.h"

// Tụi mình sẽ tạo 2 class này ở các bước sau:
// #include "MarioMap.h" 
// #include "WorldKeyEventHandler.h"

using namespace std;

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_OBJECTS	2
#define SCENE_SECTION_GRID_OBJECTS 3

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2

#define MAX_SCENE_LINE 1024

WorldScene::WorldScene(int id, LPCWSTR filePath) : Scene(id, filePath)
{
	worldMario = NULL;
	SetBackgroundColor(0.0f, 0.0f, 0.0f);
	this->key_handler = new WorldSceneKeyHandler(this);
	SetBackgroundColor(0.0f, 0.0f, 0.0f);
}

void WorldScene::Load()
{
	DebugOut(L"[INFO] Start loading World Map from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; };
		if (line == "[OBJECTS]") { section = SCENE_SECTION_OBJECTS; continue; };
		if (line == "[GRID_OBJECTS]") { section = SCENE_SECTION_GRID_OBJECTS; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_GRID_OBJECTS: _ParseSection_OBJECTS(line, true); break;
		}
	}
	f.close();

	// Set lại tọa độ cho HUD y như PlayScene
	float screenHeight = GameGlobal::GetHeight();
	HUD::GetInstance()->SetPosition(0.0f, screenHeight - HUD_HEIGHT);

	// CHÚ Ý: Ở ngoài Map thì không có đếm ngược thời gian, nạp số 0!
	GameManager::GetInstance()->ResetTimer(0);

	if (worldMario != NULL)
	{
		float cx = GameManager::GetInstance()->mapMarioCurrentX;
		float cy = GameManager::GetInstance()->mapMarioCurrentY;

		// Nếu thẻ nhớ đã ghi nhận tọa độ, thì ép Mario nằm đúng ở đó
		if (cx != -1.0f && cy != -1.0f)
		{
			worldMario->SetPosition(cx, cy);
		}
	}

	DebugOut(L"[INFO] Done loading World Map  %s\n", sceneFilePath);
}

void WorldScene::Update(DWORD dt)
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &objects);
	}

	GameManager::GetInstance()->Update(dt);

	// Tự động tính toán dựa trên kích thước map đã load
	float screenWidth = GameGlobal::GetWidth();
	float playableHeight = GameGlobal::GetHeight() - HUD_HEIGHT;

	float offset_x = (screenWidth - this->mapWidth) / 2.0f;
	float offset_y = (playableHeight - this->mapHeight) / 2.0f;

	// Nếu map to hơn hoặc bằng màn hình thì ghim Camera ở 0 (không cần canh giữa)
	if (offset_x < 0) offset_x = 0;
	if (offset_y < 0) offset_y = 0;

	Camera::GetInstance()->SetCamPos(-offset_x, -offset_y);

	PurgeDeletedObjects();
}

void WorldScene::Render()
{
	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetZIndex() < 5) objects[i]->Render();

	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetZIndex() >= 5 && objects[i]->GetZIndex() < 10)
			objects[i]->Render();

	if (worldMario != NULL) worldMario->Render();

	HUD::GetInstance()->Render();
}

void WorldScene::Unload()
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();
	worldMario = NULL;

	DebugOut(L"[INFO] Scene %d unloaded! Destroyed all objects.\n", id);
}

void WorldScene::Clear()
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	objects.clear();
}

void WorldScene::PurgeDeletedObjects()
{}

void WorldScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 6) return;

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPTEXTURE tex = Textures::GetInstance()->Get(texID);
	if (tex == NULL) {
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}
	Sprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void WorldScene::_ParseSection_ASSETS(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 1) return;
	wstring path = ToWSTR(tokens[0]);
	LoadAssets(path.c_str());
}

void WorldScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 3) return;

	LPANIMATION ani = new Animation();
	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}
	Animations::GetInstance()->Add(ani_id, ani);
}

void WorldScene::_ParseSection_OBJECTS(string line, bool isGridCoordinate)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 3) return;

	int object_type = atoi(tokens[0].c_str());
	float raw_x = (float)atof(tokens[1].c_str());
	float raw_y = (float)atof(tokens[2].c_str());

	// Áp dụng phép nhân nếu đang ở chế độ Grid, nếu không thì giữ nguyên
	float x = isGridCoordinate ? (raw_x * TILE_SIZE) : raw_x;
	float y = isGridCoordinate ? (raw_y * TILE_SIZE) : raw_y;

	GameObject* obj = NULL;

	switch (object_type)
	{
	case WORLDMAP_MAP: {
		int sprite_id = atoi(tokens[3].c_str());
		obj = new MapBackground(x, y, sprite_id);

		// Lấy kích thước thực tế của tấm ảnh Map và lưu lại
		Sprite* spr = Sprites::GetInstance()->Get(sprite_id);
		if (spr != NULL) {
			this->mapWidth = spr->GetWidth();
			this->mapHeight = spr->GetHeight();
		}
		break;
	}
	case WORLDMAP_MARIO: {
		obj = new MapMario(x, y);
		worldMario = (MapMario*)obj;
		break;
	}
	case WORLDMAP_OBJECT: {
		obj = new WorldMapObject(x, y, atoi(tokens[3].c_str()));
		break;
	}
	case WORLDMAP_NODE_INVISIBLE: {
		int l, r, u, d;
		if (tokens.size() < 7) l = r = u = d = 0;
		l = atoi(tokens[3].c_str());
		r = atoi(tokens[4].c_str());
		u = atoi(tokens[5].c_str());
		d = atoi(tokens[6].c_str());
		obj = new InvisibleNode(x, y, l, r, u, d);
		break;
	}
	case WORLDMAP_NODE_STAGE: {
		int l, r, u, d, stage, sceneId;
		if (tokens.size() < 7) l = r = u = d = 0;
		l = atoi(tokens[3].c_str());
		r = atoi(tokens[4].c_str());
		u = atoi(tokens[5].c_str());
		d = atoi(tokens[6].c_str());
		stage = atoi(tokens[7].c_str());
		sceneId = atoi(tokens[8].c_str());
		obj = new StageNode(x, y, l, r, u, d, stage, sceneId);
		break;
	}

	default:
		DebugOut(L"[ERROR] Invalid object type: %d\n", object_type);
		return;
	}

	if (obj != NULL) {
		obj->SetPosition(x, y);
		objects.push_back(obj);
	}
}

void WorldScene::LoadAssets(LPCWSTR assetFile)
{
	DebugOut(L"[INFO] Start loading assets from : %s \n", assetFile);
	ifstream f;
	f.open(assetFile);
	int section = ASSETS_SECTION_UNKNOWN;
	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);
		if (line[0] == '#') continue;
		if (line == "[SPRITES]") { section = ASSETS_SECTION_SPRITES; continue; };
		if (line == "[ANIMATIONS]") { section = ASSETS_SECTION_ANIMATIONS; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }
		switch (section)
		{
		case ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		}
	}
	f.close();
	DebugOut(L"[INFO] Done loading assets from %s\n", assetFile);
}