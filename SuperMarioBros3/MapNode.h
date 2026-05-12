#pragma once
#include "GameObject.h"

class MapNode : public GameObject
{
public:
	bool canLeft, canRight, canUp, canDown;

	MapNode(float x, float y, int l, int r, int u, int d) : GameObject(x, y)
	{
		canLeft = l;
		canRight = r;
		canUp = u;
		canDown = d;
	}

	virtual void Render() {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) {}
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b) {
		l = x; t = y; r = x + 16; b = y + 16;
	}
};