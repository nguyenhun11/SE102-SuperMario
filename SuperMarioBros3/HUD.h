#pragma once
#include "Sprites.h"
#include "Camera.h"

class PlaySceneHUD
{
protected:
	float screenX;
	float screenY;
	int spriteId;

public:
	PlaySceneHUD(float x, float y, int sprite_id);
	void Update(DWORD dt);
	void Render();
};