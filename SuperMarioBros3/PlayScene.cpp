#include <iostream>
#include <fstream>
#include "AssetIDs.h"

#include "PlayScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Coin.h"
#include "Koopa.h"
#include "Platform.h"
#include "QuestionBlock.h"
#include "Ground.h"
#include "SemisolidPlatform.h"
#include "Background.h"
#include "VerticalPipe.h"

#include "PlaySceneKeyHandler.h"

using namespace std;

PlayScene::PlayScene(int id, LPCWSTR filePath):
	Scene(id, filePath)
{
	player = NULL;
	key_handler = new PlaySceneKeyHandler(this);
}


#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_OBJECTS	2
#define SCENE_SECTION_GRID_OBJECTS	3

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2

#define MAX_SCENE_LINE 1024
#define TILE_SIZE 8.0f

void PlayScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPTEXTURE tex = Textures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return; 
	}

	Sprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void PlayScene::_ParseSection_ASSETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 1) return;

	wstring path = ToWSTR(tokens[0]);
	
	LoadAssets(path.c_str());
}

void PlayScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new Animation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < tokens.size(); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i+1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	Animations::GetInstance()->Add(ani_id, ani);
}

/*
	Parse a line in section [OBJECTS] 
*/
void PlayScene::_ParseSection_OBJECTS(string line, bool isGridCoordinate)
{
	vector<string> tokens = split(line);

	// skip invalid lines - an object set must have at least id, x, y
	if (tokens.size() < 3) return;

	int object_type = atoi(tokens[0].c_str());
	float raw_x = (float)atof(tokens[1].c_str());
	float raw_y = (float)atof(tokens[2].c_str());

	// Áp dụng phép nhân nếu đang ở chế độ Grid, nếu không thì giữ nguyên
	float x = isGridCoordinate ? (raw_x * TILE_SIZE) : raw_x;
	float y = isGridCoordinate ? (raw_y * TILE_SIZE) : raw_y;

	GameObject *obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player!=NULL) 
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		obj = new Mario(x,y); 
		player = (Mario*)obj;  

		DebugOut(L"[INFO] Player object has been created!\n");
		break;
	case OBJECT_TYPE_GOOMBA: obj = new Goomba(x,y); break;
	case OBJECT_TYPE_BRICK: obj = new Brick(x,y); break;
	case OBJECT_TYPE_COIN: obj = new Coin(x, y); break;
	case OBJECT_TYPE_KOOPA: obj = new Koopa(x, y); break;

	case OBJECT_TYPE_PLATFORM:
	{

		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());
		int length = atoi(tokens[5].c_str());
		int sprite_begin = atoi(tokens[6].c_str());
		int sprite_middle = atoi(tokens[7].c_str());
		int sprite_end = atoi(tokens[8].c_str());

		obj = new Platform(
			x, y,
			cell_width, cell_height, length,
			sprite_begin, sprite_middle, sprite_end
		);

		break;
	}
	case OBJECT_TYPE_SEMISOLID_PLATFORM:
	{
		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());

		int columns = atoi(tokens[5].c_str());
		int rows = atoi(tokens[6].c_str());

		int spriteID_tl = atoi(tokens[7].c_str());
		int spriteID_tm = atoi(tokens[8].c_str());
		int spriteID_tr = atoi(tokens[9].c_str());

		int spriteID_ml = atoi(tokens[10].c_str());
		int spriteID_mm = atoi(tokens[11].c_str());
		int spriteID_mr = atoi(tokens[12].c_str());

		int spriteID_bl = atoi(tokens[13].c_str());
		int spriteID_bm = atoi(tokens[14].c_str());
		int spriteID_br = atoi(tokens[15].c_str());

		int shadow_top = atoi(tokens[16].c_str());
		int shadow_mid = atoi(tokens[17].c_str());
		int shadow_bot = atoi(tokens[18].c_str());

		obj = new SemisolidPlatform(x, y, cell_width, cell_height, columns, rows,
			spriteID_tl, spriteID_tm, spriteID_tr,
			spriteID_ml, spriteID_mm, spriteID_mr,
			spriteID_bl, spriteID_bm, spriteID_br,
			shadow_top, shadow_mid, shadow_bot);

		break;
	}
	case OBJECT_TYPE_VERTICAL_PIPE:
	{
		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());

		int rows = atoi(tokens[5].c_str());

		int idTopLeft = atoi(tokens[6].c_str());
		int idTopRight = atoi(tokens[7].c_str());
		int idBodyLeft = atoi(tokens[8].c_str());
		int idBodyRight = atoi(tokens[9].c_str());

		obj = new VerticalPipe(x, y, cell_width, cell_height, rows,
			idTopLeft, idTopRight, idBodyLeft, idBodyRight);

		objects.push_back(obj);
		break;
	}
	case OBJECT_TYPE_QUESTION_BLOCK:
	{
		int contained_item_id = atoi(tokens[3].c_str());
		obj = new QuestionBlock(x, y, contained_item_id);
		break;

	}

	case OBJECT_TYPE_PORTAL:
	{
		float r = (float)atof(tokens[3].c_str());
		float b = (float)atof(tokens[4].c_str());
		int scene_id = atoi(tokens[5].c_str());
		obj = new Portal(x, y, r, b, scene_id);
		break;
	}

	case OBJECT_TYPE_GROUND: {

		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());
		int columns = atof(tokens[5].c_str());
		int rows = atof(tokens[6].c_str());
		int spriteID_tl = atof(tokens[7].c_str());
		int spriteID_tm = atof(tokens[8].c_str());
		int spriteID_tr = atof(tokens[9].c_str());
		int spriteID_ml = atof(tokens[10].c_str());
		int spriteID_mm = atof(tokens[11].c_str());
		int spriteID_mr = atof(tokens[12].c_str());

		obj = new Ground(x, y, cell_width, cell_height, columns, rows,
			spriteID_tl, spriteID_tm, spriteID_tr, spriteID_ml, spriteID_mm, spriteID_mr);

		break;
	}
	case OBJECT_TYPE_BACKGROUND: {
		obj = new Background(x, y, atoi(tokens[3].c_str()));
		break;
	}


	default:
		DebugOut(L"[ERROR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);


	objects.push_back(obj);
}

void PlayScene::LoadAssets(LPCWSTR assetFile)
{
	DebugOut(L"[INFO] Start loading assets from : %s \n", assetFile);

	ifstream f;
	f.open(assetFile);

	int section = ASSETS_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);
		//DebugOut(L"[READSCENE] Read: %s \n", ToWSTR(line).c_str());

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[SPRITES]") { section = ASSETS_SECTION_SPRITES; continue; };
		if (line == "[ANIMATIONS]") { section = ASSETS_SECTION_ANIMATIONS; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		}
	}

	f.close();

	DebugOut(L"[INFO] Done loading assets from %s\n", assetFile);
}

void PlayScene::Load()
{
	DebugOut(L"[INFO] Start loading scene from : %s \n", sceneFilePath);

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

	DebugOut(L"[INFO] Done loading scene  %s\n", sceneFilePath);
}

void PlayScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 1; i < objects.size(); i++)
	{
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return; 

	// Update camera to follow mario
	float cx, cy;
	player->GetPosition(cx, cy);

	// HUD space
	float hudHeight = 32.0f;
	float playableHeight = GameGlobal::GetHeight() - hudHeight;

	cx -= GameGlobal::GetWidth() / 2;
	cy -= playableHeight / 2;;

	if (cx < 0) cx = 0;
	if (cy < 0) cy = 0;

	float mapHeight = 240.0f;
	float max_cy = mapHeight - GameGlobal::GetHeight();

	if (max_cy < 0) max_cy = 0;
	if (cy > max_cy) cy = max_cy;

	Camera::GetInstance()->SetCamPos(cx, cy);

	PurgeDeletedObjects();
}

void PlayScene::Render()
{
	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetZIndex() < 5) objects[i]->Render();

	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetZIndex() >= 5 && objects[i]->GetZIndex() < 10)
			objects[i]->Render();

	player->Render();
}

/*
*	Clear all objects from this scene
*/
void PlayScene::Clear()
{
	vector<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		delete (*it);
	}
	objects.clear();
}

/*
	Unload scene

	TODO: Beside objects, we need to clean up sprites, animations and textures as well 

*/
void PlayScene::Unload()
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];

	objects.clear();
	player = NULL;

	DebugOut(L"[INFO] Scene %d unloaded! \n", id);
}

bool PlayScene::IsGameObjectDeleted(const LPGAMEOBJECT& o) { return o == NULL; }

void PlayScene::PurgeDeletedObjects()
{
	vector<LPGAMEOBJECT>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		LPGAMEOBJECT o = *it;
		if (o->IsDeleted())
		{
			delete o;
			*it = NULL;
		}
	}

	// NOTE: remove_if will swap all deleted items to the end of the vector
	// then simply trim the vector, this is much more efficient than deleting individual items
	objects.erase(
		std::remove_if(objects.begin(), objects.end(), PlayScene::IsGameObjectDeleted),
		objects.end());
}