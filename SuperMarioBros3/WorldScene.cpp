#include <iostream>
#include <fstream>
#include "AssetIDs.h"
#include "debug.h"

#include "GameGlobal.h"
#include "WorldScene.h"
#include "HUD.h"
#include "GameManager.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Camera.h"
#include "MapBackground.h"

// Tụi mình sẽ tạo 2 class này ở các bước sau:
// #include "MarioMap.h" 
// #include "WorldKeyEventHandler.h"

using namespace std;

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_OBJECTS	2

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2

#define MAX_SCENE_LINE 1024

WorldScene::WorldScene(int id, LPCWSTR filePath) : Scene(id, filePath)
{
	worldPlayer = NULL;
	// Tạm thời comment lại, bước sau tụi mình viết class này rồi mở ra
	// key_handler = new WorldKeyEventHandler(this); 
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
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}
	f.close();

	// Set lại tọa độ cho HUD y như PlayScene
	float screenHeight = GameGlobal::GetHeight();
	HUD::GetInstance()->SetPosition(0.0f, screenHeight - HUD_HEIGHT);

	// CHÚ Ý: Ở ngoài Map thì không có đếm ngược thời gian, nạp số 0!
	GameManager::GetInstance()->ResetTimer(0);

	DebugOut(L"[INFO] Done loading World Map  %s\n", sceneFilePath);
}

void WorldScene::Update(DWORD dt)
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt);
	}

	// Cập nhật GameManager (để thời gian không bị đứng)
	GameManager::GetInstance()->Update(dt);

	// Ghim Camera cứng ở góc trên cùng bên trái
	Camera::GetInstance()->SetCamPos(0.0f, 0.0f);

	PurgeDeletedObjects();
}

void WorldScene::Render()
{
	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetZIndex() < 5) objects[i]->Render();

	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetZIndex() >= 5 && objects[i]->GetZIndex() < 10)
			objects[i]->Render();

	if (worldPlayer != NULL) worldPlayer->Render();

	HUD::GetInstance()->Render();
}

void WorldScene::Unload()
{}

void WorldScene::Clear()
{}

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

void WorldScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 3) return;

	int object_type = atoi(tokens[0].c_str());
	float x = (float)atof(tokens[1].c_str());
	float y = (float)atof(tokens[2].c_str());

	GameObject* obj = NULL;

	switch (object_type)
	{
		/* Tụi mình sẽ định nghĩa OBJECT_TYPE_MARIO_MAP sau
		case OBJECT_TYPE_MARIO_MAP:
			if (player != NULL)
			{
				DebugOut(L"[ERROR] MARIO MAP was created before!\n");
				return;
			}
			obj = new MarioMap(x, y);
			player = (MarioMap*)obj;
			DebugOut(L"[INFO] Player Map object has been created!\n");
			break;
		*/
	case WORLDMAP_MAP: {
		obj = new MapBackground(x, y, atoi(tokens[3].c_str()));
		break;
	}
	// Sau này sẽ thêm case cho OBJECT_TYPE_NODE (Trạm dừng) ở đây...

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