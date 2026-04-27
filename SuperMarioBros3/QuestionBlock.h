#pragma once
#include "GameObject.h"

#define ID_ANIM_QUESTION_BLOCK_ACTIVE	3020
#define ID_ANIM_QUESTION_BLOCK_EMPTY	3021

#define QUESTION_BLOCK_BBOX_WIDTH 16
#define QUESTION_BLOCK_BBOX_HEIGHT 16

// bounce
#define QBLOCK_BOUNCE_SPEED 0.15f
#define QBLOCK_BOUNCE_HEIGHT 10.0f

enum class CONTAINED_OBJECT {
	COIN,
	MUSHROOM,
	LEAF,
	UP_MUSHROOM 
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
	CONTAINED_OBJECT item;
	QuestionBlockState currentState;

public:
	QuestionBlock(float x, float y, CONTAINED_OBJECT containedItem = CONTAINED_OBJECT::COIN);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	virtual void SetState(QuestionBlockState state);

	virtual int IsBlocking() { return 1; }

	QuestionBlockState GetCurrentState() { return currentState; }
};