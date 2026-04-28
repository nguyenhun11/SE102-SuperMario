#pragma once
#include "GameObject.h"

// animations
#define ID_ANIM_QUESTION_BLOCK_ACTIVE	3020
#define ID_ANIM_QUESTION_BLOCK_EMPTY	3021

// boundary box
#define QUESTION_BLOCK_BBOX_WIDTH 16
#define QUESTION_BLOCK_BBOX_HEIGHT 16

// bounce
#define QBLOCK_BOUNCE_SPEED 0.2f
#define QBLOCK_BOUNCE_HEIGHT 10.0f

enum class ContainedItem {
	COIN = 0,
	POWER_UP = 1,
	UP_MUSHROOM = 2 
};

enum class QuestionBlockState {
	ACTIVE = 0,   
	BOUNCING = 1, 
	EMPTY = 2     
};

class QuestionBlock : public GameObject
{
protected:
	float startY; 
	ContainedItem item;
	QuestionBlockState currentState;

public:
	QuestionBlock(float x, float y, int containedItem = 0);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual void SetState(QuestionBlockState state);

	int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 1; }

	void SpawnItem();

	QuestionBlockState GetCurrentState() { return currentState; }
};