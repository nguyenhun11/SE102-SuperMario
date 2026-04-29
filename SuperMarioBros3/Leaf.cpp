#include "Leaf.h"

Leaf::Leaf(float x, float y) : GameObject(x, y)
{
	this->startX = x;
	this->startY = y;
	SetState(LeafState::SPAWNING);
	zIndex = 5;
}

void Leaf::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (state == static_cast<int>(LeafState::SPAWNING))
	{
		y += vy * dt;
		if (startY - y >= 40.0f) // Lá bay lên cao tầm chắc cỡ 2.5 ô gạch, mỗi tile là tầm 16px
		{
			SetState(LeafState::FALLING);
		}
	}
	else if (state == static_cast<int>(LeafState::FALLING))
	{
		y += vy * dt;
		// vx thay đổi theo hàm sin của quãng đường rơi y
		vx = LEAF_HORIZONTAL_SPEED * sin((y - startY) * 0.1f);
		x += vx * dt;
	}
}

void Leaf::Render()
{
	int aniId = ID_ANIM_LEAF;
	LPANIMATION ani = Animations::GetInstance()->Get(aniId);
	if (ani) ani->Render(x, y, (vx > 0)); // flip hình
}

void Leaf::SetState(LeafState newState)
{
	switch (newState)
	{
	case LeafState::SPAWNING:
		vy = -LEAF_SPAWN_SPEED_Y;
		vx = 0;
		break;
	case LeafState::FALLING:
		vy = LEAF_FALLING_SPEED_Y;
		break;
	}
	GameObject::SetState(static_cast<int>(newState));
}

void Leaf::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - LEAF_BBOX_WIDTH / 2;
	t = y - LEAF_BBOX_HEIGHT / 2;
	r = l + LEAF_BBOX_WIDTH;
	b = t + LEAF_BBOX_HEIGHT;
}