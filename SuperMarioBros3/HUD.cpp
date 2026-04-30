#include "HUD.h"

PlaySceneHUD::PlaySceneHUD(float x, float y, int sprite_id)
{
	this->screenX = x;
	this->screenY = y;
	this->spriteId = sprite_id;
}

void PlaySceneHUD::Update(DWORD dt)
{
	// Code update sau này
}

void PlaySceneHUD::Render()
{
	float cx, cy;
	Camera::GetInstance()->GetCamPos(cx, cy);

	float drawX = cx + screenX;
	float drawY = cy + screenY;

	Sprites::GetInstance()->Get(spriteId)->Draw(drawX, drawY);
}