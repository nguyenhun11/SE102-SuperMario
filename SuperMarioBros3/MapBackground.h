#pragma once
#include "GameObject.h"

class MapBackground : public GameObject
{
protected:
	int spriteId;
public:
	MapBackground(float x, float y, int spriteId);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) {}
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b) {}
};