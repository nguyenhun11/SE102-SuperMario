#pragma once
#include "GameObject.h"

enum class InvisibleType
{
	NOTE_BLOCK = 0,
	QUESTION_BLOCK = 1
};

class InvisibleBlock : public GameObject
{
	InvisibleType type;
	bool isTriggered;

public:
	InvisibleBlock(float x, float y, InvisibleType type);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	bool IsTriggered() { return isTriggered; }
	void Trigger();

	virtual int IsBlocking() { return 0; }
	virtual int IsCollidable() { return 1; }
};