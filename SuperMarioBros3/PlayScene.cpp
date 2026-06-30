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
#include "NoteBlock.h"
#include "GoalBlock.h"
#include "Ground.h"
#include "Slope.h"
#include "SemisolidPlatform.h"
#include "Decoration.h"
#include "VerticalPipe.h"
#include "HorizontalPipe.h"
#include "SolidBlock.h"
#include "SoundManager.h"
#include "Switch.h"
#include "PiranhaPlant.h"
#include "KoopaTroopa.h"
#include "BoomerangBro.h"

#include "PlaySceneKeyHandler.h"

using namespace std;

PlayScene::PlayScene(int id, LPCWSTR filePath) :
	Scene(id, filePath)
{
	player = NULL;
	key_handler = new PlaySceneKeyHandler(this);
}


#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS	1
#define SCENE_SECTION_OBJECTS	2
#define SCENE_SECTION_GRID_OBJECTS	3
#define SCENE_SECTION_MAP_INFO	4
#define SCENE_SECTION_CAMERA_ZONES 5

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2

#define MAX_SCENE_LINE 1024
#define TILE_SIZE 16.0f

void PlayScene::_ParseSection_MAP_INFO(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 1) return;
	mapRight = (float)atof(tokens[0].c_str());
}

void PlayScene::_ParseSection_CAMERA_ZONES(string line)
{
	vector<string> tokens = split(line);
	if (tokens.size() < 4) return;
	CameraZone zone;
	zone.left = (float)atof(tokens[0].c_str());
	zone.top = (float)atof(tokens[1].c_str());
	zone.right = (float)atof(tokens[2].c_str());
	zone.bottom = (float)atof(tokens[3].c_str());
	cameraZones.push_back(zone);
}

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
		int frame_time = atoi(tokens[i + 1].c_str());
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

	GameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		obj = new Mario(x, y);
		player = (Mario*)obj;


		DebugOut(L"[INFO] Player object has been created!\n");
		break;
	case OBJECT_TYPE_SOLID_BLOCK:
	{
		int sprite_id = atoi(tokens[3].c_str());
		obj = new SolidBlock(x, y, sprite_id);
		break;
	}
	case OBJECT_TYPE_GOOMBA: obj = new Goomba(x, y); break;
	case OBJECT_TYPE_BOOMERANG_BRO:
		obj = new BoomerangBro(x, y);
		break;
	case OBJECT_TYPE_PIRANHA_PLANT: obj = new PiranhaPlant(x, y); break;
	case OBJECT_TYPE_BRICK:
	{
		int item_type = 0;
		int switch_type = 0;

		if (tokens.size() > 3)
		{
			item_type = atoi(tokens[3].c_str());
		}
		if (tokens.size() > 4)
		{
			switch_type = atoi(tokens[4].c_str());
		}
		obj = new Brick(x, y, item_type, switch_type);
		break;
	}
	case OBJECT_TYPE_COIN: obj = new Coin(x, y); break;
	case OBJECT_TYPE_KOOPA: 
	{
		int color = 0; // 0 là GREEN, 1 là RED
		if (tokens.size() > 3)
		{
			color = atoi(tokens[3].c_str());
		}
		obj = new Koopa(x, y, static_cast<KoopaColor>(color));
		break;
	}
	case OBJECT_TYPE_KOOPATROOPA:
	{
		int color = 0; // 0 là GREEN, 1 là RED
		if (tokens.size() > 3)
		{
			color = atoi(tokens[3].c_str());
		}
		obj = new KoopaTroopa(x, y, static_cast<KoopaColor>(color));
		break;
	}

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

		int shadow_topB, shadow_midB, shadow_botB;

		if (tokens.size() > 19)
		{
			int shadow_topB = atoi(tokens[19].c_str());
			int shadow_midB = atoi(tokens[20].c_str());
			int shadow_botB = atoi(tokens[21].c_str());
			obj = new SemisolidPlatform(x, y, cell_width, cell_height, columns, rows,
				spriteID_tl, spriteID_tm, spriteID_tr,
				spriteID_ml, spriteID_mm, spriteID_mr,
				spriteID_bl, spriteID_bm, spriteID_br,
				shadow_top, shadow_mid, shadow_bot,
				shadow_topB, shadow_midB, shadow_botB);
		}
		else
		{
		obj = new SemisolidPlatform(x, y, cell_width, cell_height, columns, rows,
			spriteID_tl, spriteID_tm, spriteID_tr,
			spriteID_ml, spriteID_mm, spriteID_mr,
			spriteID_bl, spriteID_bm, spriteID_br,
			shadow_top, shadow_mid, shadow_bot);
		}

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
		int idBottomLeft = atoi(tokens[10].c_str());
		int idBottomRight = atoi(tokens[11].c_str());

		int isBlock = 1;
		if (tokens.size() > 12) isBlock = atoi(tokens[12].c_str());

		int targetScene = -1;
		if (tokens.size() > 13) targetScene = atoi(tokens[13].c_str());

		int contentType = 0;
		if (tokens.size() > 14) contentType = atoi(tokens[14].c_str());

		float contentHeight = 32.0f;
		if (tokens.size() > 15) contentHeight = (float)atof(tokens[15].c_str());

		VerticalPipe* pipe = new VerticalPipe(x, y, cell_width, cell_height, rows,
			idTopLeft, idTopRight, idBodyLeft, idBodyRight, idBottomLeft, idBottomRight, isBlock, targetScene, contentType, contentHeight);
		
		pipe->SpawnContent();
		obj = pipe;

		break;
	}
	case OBJECT_TYPE_HORIZOLTAL_PIPE:
	{
		float cell_width = (float)atof(tokens[3].c_str());
		float cell_height = (float)atof(tokens[4].c_str());

		int columns = atoi(tokens[5].c_str());

		int idTopLeft = atoi(tokens[6].c_str());
		int idTop = atoi(tokens[7].c_str());
		int idTopRight = atoi(tokens[8].c_str());
		int idBottomLeft = atoi(tokens[9].c_str());
		int idBottom = atoi(tokens[10].c_str());
		int idBottomRight = atoi(tokens[11].c_str());

		int isBlock = 1;
		if (tokens.size() > 12) isBlock = atoi(tokens[12].c_str());

		int targetScene = -1;
		if (tokens.size() > 13) targetScene = atoi(tokens[13].c_str());

		int contentType = 0;
		if (tokens.size() > 14) contentType = atoi(tokens[14].c_str());

		int spawnSide = -1;
		if (tokens.size() > 15) spawnSide = atoi(tokens[15].c_str());

		HorizontalPipe* pipe = new HorizontalPipe(x, y, cell_width, cell_height, columns,
			idTopLeft, idTop, idTopRight,
			idBottomLeft, idBottom, idBottomRight,
			isBlock, targetScene, contentType, spawnSide);

		obj = pipe;

		break;
	}
	case OBJECT_TYPE_QUESTION_BLOCK:
	{
		int contained_item_id = atoi(tokens[3].c_str());
		obj = new QuestionBlock(x, y, contained_item_id);
		break;

	}

	case OBJECT_TYPE_NOTE_BLOCK:
	{
		int contained_item_id = 0;
		int bounceCount = 1;
		contained_item_id = atoi(tokens[3].c_str());
		if (tokens.size() > 4)
		{
			bounceCount = atoi(tokens[4].c_str());
		}

		obj = new NoteBlock(x, y, contained_item_id, bounceCount);
		break;
	}

	case OBJECT_TYPE_GOAL_BLOCK:
	{
		obj = new GoalBlock(x, y);
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


						   /// Cái joke
	case OBJECT_TYPE_SLOPE:
	{
		//float width = (float)atof(tokens[3].c_str());
		//float height = (float)atof(tokens[4].c_str());
		//int dir = atoi(tokens[5].c_str());

		//float center_x = x + (width / 2.0f);
		//float center_y = y + (height / 2.0f);

		//obj = new Slope(center_x, center_y, width, height, dir);
		//break;
		float width = (float)atof(tokens[3].c_str());
		float height = (float)atof(tokens[4].c_str());
		int dir = atoi(tokens[5].c_str());

		// LƯU Ý: Vì đọc từ [GRID_OBJECTS], x và y đã được nhân với TILE_SIZE (16) ở trên rồi.
		// Công việc của bạn chỉ là Dịch tọa độ về TÂM cho khớp với hệ thống:
		float center_x = x + (width / 2.0f);
		float center_y = y + (height / 2.0f);

		obj = new Slope(center_x, center_y, width, height, dir);
		break;
	}

	case OBJECT_TYPE_DECORATION: {
		obj = new Decoration(x, y, atoi(tokens[3].c_str()));
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

	//// LOAD SFX & BGM
	// Tại nơi khởi tạo game
	SoundManager::GetInstance()->Init();

	// Load các file âm thanh (đảm bảo file tồn tại trong thư mục assets)
	SoundManager::GetInstance()->Load("bgm_stage1", "assets/sounds/bgm_stage1.wav");

	SoundManager::GetInstance()->Load("1up", "assets/sounds/smb3_1-up.wav");
	SoundManager::GetInstance()->Load("coin", "assets/sounds/smb3_coin.wav");
	SoundManager::GetInstance()->Load("jump", "assets/sounds/smb3_jump.wav");
	SoundManager::GetInstance()->Load("skid", "assets/sounds/smb3_skid.wav");
	SoundManager::GetInstance()->Load("kick", "assets/sounds/smb3_kick.wav");
	SoundManager::GetInstance()->Load("bump", "assets/sounds/smb3_bump.wav");
	SoundManager::GetInstance()->Load("brick_break", "assets/sounds/smb3_brick_break.wav");
	SoundManager::GetInstance()->Load("level_clear", "assets/sounds/smb3_level_clear.wav");
	SoundManager::GetInstance()->Load("pipe", "assets/sounds/smb3_pipe.wav");
	SoundManager::GetInstance()->Load("player_down", "assets/sounds/smb3_player_down.wav");
	SoundManager::GetInstance()->Load("power_up", "assets/sounds/smb3_power-up.wav");
	SoundManager::GetInstance()->Load("score", "assets/sounds/smb3_inventory.wav");
	SoundManager::GetInstance()->Load("mushroom_appear", "assets/sounds/smb3_mushroom_appear.wav");
	SoundManager::GetInstance()->Load("racoon", "assets/sounds/smb3_raccoon_transform.wav");
	SoundManager::GetInstance()->Load("tail", "assets/sounds/smb3_tail.wav");
	SoundManager::GetInstance()->Load("pmeter", "assets/sounds/smb3_pmeter.wav");
	SoundManager::GetInstance()->Load("stomp", "assets/sounds/smb3_stomp.wav");
	// Phát nhạc nền ngay khi vào màn chơi
	SoundManager::GetInstance()->PlayBGM("bgm_stage1");

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
		if (line == "[MAP_INFO]") { section = SCENE_SECTION_MAP_INFO; continue; };
		if (line == "[CAMERA_ZONES]") { section = SCENE_SECTION_CAMERA_ZONES; continue; };
		if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; };
		if (line == "[OBJECTS]") { section = SCENE_SECTION_OBJECTS; continue; };
		if (line == "[GRID_OBJECTS]") { section = SCENE_SECTION_GRID_OBJECTS; continue; };
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_MAP_INFO: _ParseSection_MAP_INFO(line); break;
		case SCENE_SECTION_CAMERA_ZONES: _ParseSection_CAMERA_ZONES(line); break;
		case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		case SCENE_SECTION_GRID_OBJECTS: _ParseSection_OBJECTS(line, true); break;
		}
	}
	f.close();


	//  ĐÚNG: Gọi thông qua Instance duy nhất
	GameManager* gm = GameManager::GetInstance();
	if (gm->isGoingThroughPipe && player != NULL)
	{
		// 1. Tìm cái cống đầu tiên trong map được thiết kế làm "Cửa Ra"
		VerticalPipe* exitPipe = NULL;
		for (size_t i = 0; i < objects.size(); i++)
		{
			if (dynamic_cast<VerticalPipe*>(objects[i]))
			{
				VerticalPipe* pipe = dynamic_cast<VerticalPipe*>(objects[i]);
				if (pipe->GetTargetSceneId() == 999)
				{
					exitPipe = pipe;
					break;
				}
			}
		}
		if (exitPipe != NULL)
		{
			float pl, pt, pr, pb;
			exitPipe->GetBoundingBox(pl, pt, pr, pb);

			float spawnX = pl + (pr - pl) / 2;
			float spawnY = pt + 16.0f;

			player->SetPosition(spawnX, spawnY);

			Mario* mario = static_cast<Mario*>(player);
			mario->SetStartPiping();
		}
	}
	gm->isGoingThroughPipe = false;

	float screenHeight = GameGlobal::GetHeight();
	HUD::GetInstance()->SetPosition(0.0f, screenHeight - HUD_HEIGHT);
	GameManager::GetInstance()->ResetTimer(300000);

	DebugOut(L"[INFO] Done loading scene  %s\n", sceneFilePath);
}

void PlayScene::Update(DWORD dt)
{
	for (size_t i = 0; i < objects.size(); i++)
	{
		LPGAMEOBJECT obj = objects[i];
		bool isOnCamera = obj->IsOnCamera();

		if (isOnCamera && !obj->GetWasOnCamera())
		{
			obj->OnEnterCamera();
		}
		else if (!isOnCamera && obj->GetWasOnCamera())
		{
			obj->OnExitCamera();
		}

		obj->SetWasOnCamera(isOnCamera);
	}

	vector<LPGAMEOBJECT> coObjects;
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i] == player || !objects[i]->GetActive()) continue;
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->GetActive())
		{
			objects[i]->Update(dt, &coObjects);
		}
	}

	GameManager::GetInstance()->Update(dt);

	//--- PLAYER POSITION
	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;
	Mario* mario = static_cast<Mario*>(player);

	// set vị trí mai rùa mà mario cầm, đoạn này code dơ, thông cảm thông cảm
	if (mario != NULL && mario->heldKoopa != NULL && mario->isHolding)
	{
		Koopa* koopa = mario->heldKoopa;
		koopa->isHeld = true;
		float hx = mario->GetX() + mario->GetDirection() * 12.0f;
		float hy = mario->GetY() - 2.0f;
		koopa->SetPosition(hx, hy);
	}


	float px, py;
	player->GetPosition(px, py);



	float max_player_x = this->mapRight * TILE_SIZE - 8.0f;
	if (mario->IsGoalRunning())
	{
		max_player_x = 999 * TILE_SIZE;
	}


	if (px < mapLeft)
	{
		px = mapLeft;
		player->SetPosition(px, py); // Khóa Y, ép X quay lại mép trái

		float pvx, pvy;
		player->GetSpeed(pvx, pvy);
		player->SetSpeed(0.0f, pvy);
	}
	else if (px > max_player_x)			/// chặn phải
	{
		px = max_player_x;
		player->SetPosition(px, py);
		float pvx, pvy;
		player->GetSpeed(pvx, pvy);
		player->SetSpeed(0.0f, pvy);
	}
	float playerCeiling = mapTop - 64.0f;

	if (py < playerCeiling)
	{
		py = playerCeiling;
		player->SetPosition(px, py);

		float pvx, pvy;
		player->GetSpeed(pvx, pvy);
		if (pvy < 0)
		{
			player->SetSpeed(pvx, 0.0f);
		}
	}
	float deathZone = mapBottom * TILE_SIZE + 48.0f; // Rot xuong 48px la die
	if (!mario->IsGoalRunning() && py > deathZone)
	{
		/*GameManager::GetInstance()->AddLife(-1);
		Mario* mario = dynamic_cast<Mario*>(player);
		mario->Reset();*/
		GameManager::GetInstance()->LevelFailed();
		SoundManager::GetInstance()->StopAll();
	}

	//--- FOLLOW CAMERA
	float cx = px, cy = py;
	CameraZone currentZone = GetZoneX(px);
	if (prevCameraZone.left == defaultCameraZone.left && prevCameraZone.top == defaultCameraZone.top &&
		prevCameraZone.right == defaultCameraZone.right && prevCameraZone.bottom == defaultCameraZone.bottom)
	{
		prevCameraZone = currentZone;
	}
	if (currentZone.left != prevCameraZone.left || currentZone.right != prevCameraZone.right)
	{
		prevCameraZone = currentZone;
		isTransitioningCamera = true;
		cameraTransitionTimer = 0.0f;
		startCamY = currentCamY;
	}

	// HUD space
	float hudHeight = HUD_HEIGHT;
	float playableHeight = GameGlobal::GetHeight() - hudHeight;

	cx -= GameGlobal::GetWidth() / 2;
	cy -= playableHeight / 2;


	float max_cx = mapRight * TILE_SIZE - GameGlobal::GetWidth();
	float min_cx = mapLeft * TILE_SIZE;
	float min_cy = currentZone.top * TILE_SIZE;
	float max_cy = currentZone.bottom * TILE_SIZE - GameGlobal::GetHeight();
	if (cx < min_cx) cx = min_cx;
	if (cx > max_cx) cx = max_cx;

	// Clamp cy theo zone
	float targetCamY = cy;
	if (currentCamY < 0.0f)
	{
		currentCamY = targetCamY;
	}
	if (targetCamY < min_cy) targetCamY = min_cy;
	if (targetCamY > max_cy) targetCamY = max_cy;

	// Lerp camera position
	if (isTransitioningCamera)
	{
		cameraTransitionTimer += dt;
		float t = cameraTransitionTimer / CAMERA_TRANSITION_TIME;
		if (t >= 1.0f)
		{
			t = 1.0f;
			isTransitioningCamera = false;
		}
		float smooth_t = t * t * (3.0f - 2.0f * t);
		currentCamY = startCamY + (targetCamY - startCamY) * smooth_t;
	}
	else
	{
		currentCamY = targetCamY;
	}

	// Clamp currentCamY theo zone
	if (currentCamY < min_cy) currentCamY = min_cy;
	if (currentCamY > max_cy) currentCamY = max_cy;

	if (!mario->IsGoalRunning())
	{
		Camera::GetInstance()->SetCamPos(cx, currentCamY);
	}

	if (isPSwitchActive)
	{
		if (GetTickCount64() - pSwitchTimer > SWITCH_ACTIVATION_TIME)
		{
			//DebugOut(L"[PSWITCH] ---> ĐÃ HẾT GIỜ! Gọi DeactivatePSwitch()\n");
			DeactivatePSwitch(currentSwitchType);
		}

		//DebugOut(L"[INFO] P-Switch active for %lld ms\n", GetTickCount64() - pSwitchTimer);
	}

	PurgeDeletedObjects();
}

void PlayScene::Render()
{
	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetActive()
			&& objects[i]->GetZIndex() < 5)
			objects[i]->Render();

	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetActive()
			&& objects[i]->GetZIndex() >= 5
			&& objects[i]->GetZIndex() < 10)
			objects[i]->Render();

	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->GetActive()
			&& objects[i]->GetZIndex() >= 10
			&& objects[i]->GetZIndex() < 15)
			objects[i]->Render();

	//player->Render();
	HUD::GetInstance()->Render();
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

void PlayScene::ActivatePSwitch(SwitchType type)
{
	isPSwitchActive = true;
	currentSwitchType = type;
	pSwitchTimer = GetTickCount64();
	SoundManager::GetInstance()->Play("bump");
	vector<LPGAMEOBJECT> newObjects;

	for (size_t i = 0; i < objects.size(); i++)
	{
		LPGAMEOBJECT obj = objects[i];

		if (type == SwitchType::BrickToCoin && dynamic_cast<Brick*>(obj)
			&& ((Brick*)obj)->GetCurrentState() == BrickState::ACTIVE)
		{
			// Xóa viên gạch
			obj->Delete();

			// Sinh ra đồng tiền tại vị trí đó
			Coin* c = new Coin(obj->GetX(), obj->GetY());
			c->isCreatedBySwitch = true; // Đánh dấu đây là đồ giả
			newObjects.push_back(c);
		}
		else if (type == SwitchType::CoinToBrick && dynamic_cast<Coin*>(obj))
		{
			// Xóa đồng tiền
			obj->Delete();

			// Sinh ra viên gạch
			Brick* b = new Brick(obj->GetX(), obj->GetY());
			b->isCreatedBySwitch = true;
			newObjects.push_back(b);
		}
	}

	// Đổ các object mới tạo vào danh sách chính
	for (auto newObj : newObjects) {
		objects.push_back(newObj);
	}
}

void PlayScene::DeactivatePSwitch(SwitchType type)
{
	isPSwitchActive = false;
	vector<LPGAMEOBJECT> newObjects;

	int fakeCount = 0;     // Đếm số lượng "hàng giả" tìm thấy
	int deletedCount = 0;  // Đếm số lượng đã xóa và biến đổi thành công

	DebugOut(L"[DeactivatePSwitch] Bat dau quet %d objects...\n", objects.size());

	for (size_t i = 0; i < objects.size(); i++)
	{
		LPGAMEOBJECT obj = objects[i];

		// Chỉ thao tác trên những Object có cờ "Hàng giả"
		if (obj->isCreatedBySwitch)
		{
			fakeCount++; // Đã tìm thấy 1 món!

			// Thay currentSwitchType bằng biến 'type' được truyền vào cho an toàn tuyệt đối
			if (type == SwitchType::BrickToCoin && dynamic_cast<Coin*>(obj))
			{
				obj->Delete();
				Brick* b = new Brick(obj->GetX(), obj->GetY());
				b->SetState(BrickState::ACTIVE); // MỘT SỐ ENGINE CẦN DÒNG NÀY (VD: BRICK_STATE_ACTIVE) ĐỂ GẠCH HIỆN LÊN
				newObjects.push_back(b);
				deletedCount++;
			}
			else if (type == SwitchType::CoinToBrick && dynamic_cast<Brick*>(obj))
			{
				obj->Delete();
				Coin* c = new Coin(obj->GetX(), obj->GetY());
				newObjects.push_back(c);
				deletedCount++;
			}
		}
	}

	for (auto newObj : newObjects) {
		objects.push_back(newObj);
	}

	// DÒNG KẾT LUẬN QUAN TRỌNG NHẤT
	DebugOut(L"[DeactivatePSwitch] Tim thay %d hang gia | Da bien doi %d mon\n", fakeCount, deletedCount);
}

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

CameraZone PlayScene::GetZoneX(float x)
{
	for (auto& zone : cameraZones)
	{
		if (x >= zone.left * TILE_SIZE && x <= zone.right * TILE_SIZE)
		{
			return zone;
		}
	}
	return { mapLeft, mapTop, mapRight, mapBottom };
}
