#include "OneUpMushroom.h"

void OneUpMushroom::Render()
{
	LPANIMATION ani = Animations::GetInstance()->Get(ID_ANIM_ONE_UP_MUSHROOM);
	if (ani == NULL)
	{
		return;
	}
	ani->Render(x, y);

	//RenderBoundingBox(); // debug bounding shape
}

void OneUpMushroom::SetState(MushroomState newState)
{
	switch (newState)
	{
	case MushroomState::SPAWNING:
		vy = -MUSHROOM_SPAWN_SPEED;
		vx = 0;
		break;
	case MushroomState::WALKING:
		PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
		Mario* mario = (Mario*)playScene->GetPlayer();
		vy = 0;
		if (dir > 0)
			vx = MUSHROOM_WALKING_SPEED;
		else if(dir < 0)
			vx = -MUSHROOM_WALKING_SPEED;
		break;
	}

	GameObject::SetState(static_cast<int>(newState));
}
void OneUpMushroom::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (dynamic_cast<Mario*>(e->obj)) return;

	if (!e->obj->IsBlocking()) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
}