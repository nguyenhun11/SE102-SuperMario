#include "BrickDebris.h"
#include "Animations.h"
#include "Game.h" // Dùng để lấy chiều cao màn hình cho việc xóa mảnh vỡ

BrickDebris::BrickDebris(float x, float y, float vx, float vy) : GameObject(x, y)
{
	this->vx = vx;
	this->vy = vy;
	this->ax = 0;
	this->ay = DEBRIS_GRAVITY;
	this->zIndex = 6; // Đè lên các object khác cho đẹp
}

void BrickDebris::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	// Logic vật lý siêu cơ bản (Parabol)
	vy += ay * dt;
	vx += ax * dt;

	x += vx * dt;
	y += vy * dt;

	// Lấy tọa độ Camera để so sánh
	float cx, cy;
	Camera::GetInstance()->GetCamPos(cx, cy);

	// Nếu mảnh vỡ rớt xuống vượt quá cạnh dưới màn hình -> XÓA
	if (y > cy + GameGlobal::GetHeight())
	{
		this->Delete();
	}
}

void BrickDebris::Render()
{
	Animations::GetInstance()->Get(ID_ANI_BRICK_DEBRIS)->Render(x, y);
}