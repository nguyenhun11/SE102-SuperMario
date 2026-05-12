#include "Switch.h"
#include "Animations.h"
#include "SceneManager.h"
#include "PlayScene.h"

void Switch::Render()
{
	Animations* animation_set = Animations::GetInstance();
	if (type == SwitchType::BrickToCoin)
	{
		if (state == SwitchState::Off)
			animation_set->Get(BRICK_TO_COIN_OFF_ANI_ID)->Render(x, y);
		else
			animation_set->Get(BRICK_TO_COIN_ON_ANI_ID)->Render(x, y);
	}
	else
	{
		if (state == SwitchState::Off)
			animation_set->Get(COIN_TO_BRICK_OFF_ANI_ID)->Render(x, y);
		else
			animation_set->Get(COIN_TO_BRICK_ON_ANI_ID)->Render(x, y);
	}
}

void Switch::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x - 16 / 2;
	top = y - 16 / 2;
	right = left + 16;
	bottom = top + 16;
}
