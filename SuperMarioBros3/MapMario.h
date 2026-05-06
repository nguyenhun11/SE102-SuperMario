#pragma once
#include "GameObject.h"
#include "MapNode.h"
#include "SceneManager.h"
#include "StageNode.h"

#define MARIO_MAP_STATE_IDLE			0
#define MARIO_MAP_STATE_WALKING_LEFT	100
#define MARIO_MAP_STATE_WALKING_RIGHT	200
#define MARIO_MAP_STATE_WALKING_UP		300
#define MARIO_MAP_STATE_WALKING_DOWN	400

#define MARIO_MAP_SPEED		0.1f

#define ID_SPRITE_MAPMARIO_SMALL	80101

enum class MarioMapForm {
	SMALL = 0,  
	SUPER = 1,  
	RACOON = 2, 
	P = 3      
};

class MapMario : public GameObject
{
protected:
	int nx;					// 1: Phải, -1: Trái
	bool isMoving;			
	MapNode* currentNode;	

	MarioMapForm form;
	
	bool renderFlip;
	ULONGLONG flipTimer;

	ULONGLONG stopTimer;

public:
	MapMario(float x, float y);

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL);
	virtual void Render();
	virtual void SetState(int state);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	void SetMarioForm(MarioMapForm f) { this->form = f; }

	bool IsMoving() { return isMoving; }
	void MoveLeft();
	void MoveRight();
	void MoveUp();
	void MoveDown();

	void EnterNode();
};