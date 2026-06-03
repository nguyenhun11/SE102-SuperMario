#pragma once
#include "GameObject.h"

class WorldMapObject : public GameObject
{
protected:
	int aniId;
public:
	WorldMapObject(float x, float y, int aniId);

	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) {}
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b) {}
};