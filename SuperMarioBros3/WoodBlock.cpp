#include "WoodBlock.h"
#include "PlayScene.h"
#include "Mushroom.h"
#include "Leaf.h"
#include "OneUpMushroom.h"
#include "Sprites.h"

WoodBlock::WoodBlock(float x, float y, int itemType) : GameObject(x, y)
{
	this->startX = x;
	this->startY = y;
	this->isBouncingX = false;
	this->isReturningX = false;
	this->isEmptied = false;

	this->currentState = WoodBlockState::ACTIVE;
	this->state = static_cast<int>(WoodBlockState::ACTIVE);
	this->vy = 0;
	this->zIndex = 5;
	this->containedItem = static_cast<WoodBlockItem>(itemType);

	PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	WoodBlockSensor* leftSensor = new WoodBlockSensor(x - 9.0f, y, this, -1.0f);
	playScene->AddObject(leftSensor);

	WoodBlockSensor* rightSensor = new WoodBlockSensor(x + 9.0f, y, this, 1.0f);
	playScene->AddObject(rightSensor);
}

void WoodBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	x += vx * dt;
	y += vy * dt;

	if (currentState == WoodBlockState::BOUNCING)
	{
		if (isBouncingX)
		{
			if (!isReturningX)
			{
				if (abs(x - startX) >= WOODBLOCK_BOUNCE_WIDTH)
				{
					vx = -vx;          
					isReturningX = true; 
				}
			}
			else
			{
				if ((vx > 0 && x >= startX) || (vx < 0 && x <= startX))
				{
					x = startX;
					vx = 0;
					isBouncingX = false;
					isReturningX = false; 

					if (containedItem != WoodBlockItem::NONE) {
						SpawnItem();
						containedItem = WoodBlockItem::NONE;
						isEmptied = true;
						SetState(WoodBlockState::EMPTY);
					}
					else {
						if (isEmptied) SetState(WoodBlockState::EMPTY);
						else SetState(WoodBlockState::ACTIVE);
					}
				}
			}
		}
		else
		{
			if (startY - y >= WOODBLOCK_BOUNCE_HEIGHT && vy < 0) {
				vy = WOODBLOCK_BOUNCE_SPEED;
			}
			else if (y >= startY && vy > 0) {
				y = startY;
				vy = 0;

				if (containedItem != WoodBlockItem::NONE) {
					SpawnItem();
					containedItem = WoodBlockItem::NONE;
					isEmptied = true;
					SetState(WoodBlockState::EMPTY);
				}
				else {
					if (isEmptied) SetState(WoodBlockState::EMPTY);
					else SetState(WoodBlockState::ACTIVE);
				}
			}
		}
	}
}

void WoodBlock::SpawnItem()
{
	PlayScene* playScene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	Mario* mario = (Mario*)playScene->GetPlayer();

	if (this->containedItem == WoodBlockItem::POWER_UP)
	{
		if (mario->GetCurrentForm() == MarioForm::SMALL)
		{
			Mushroom* mushroom = new Mushroom(x, y);
			playScene->AddObject(mushroom);
		}
		else
		{
			Leaf* leaf = new Leaf(x, y); 
			playScene->AddObject(leaf);
		}
	}
	else if (this->containedItem == WoodBlockItem::ONE_UP_MUSHROOM)
	{
		OneUpMushroom* upMushroom = new OneUpMushroom(x, y);
		playScene->AddObject(upMushroom);
	}
}

void WoodBlock::Render()
{
	int sprId = ID_SPR_WOODBLOCK_ACTIVE;

	if (currentState == WoodBlockState::BOUNCING)
	{
		sprId = ID_SPR_WOODBLOCK_EMPTY;
	}

	Sprites::GetInstance()->Get(sprId)->DrawOnCamera(x, y);
}

void WoodBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - WOODBLOCK_BBOX_WIDTH / 2;
	t = y - WOODBLOCK_BBOX_HEIGHT / 2;
	r = l + WOODBLOCK_BBOX_WIDTH;
	b = t + WOODBLOCK_BBOX_HEIGHT;
}

void WoodBlock::SetState(WoodBlockState state)
{
	this->currentState = state;

	switch (state)
	{
	case WoodBlockState::BOUNCING:
		if (!isBouncingX)
			vy = -WOODBLOCK_BOUNCE_SPEED;
		break;
	case WoodBlockState::EMPTY:
	case WoodBlockState::ACTIVE:
		vx = 0;
		vy = 0;
		break;
	}

	GameObject::SetState(static_cast<int>(state));
}

void WoodBlock::HitHorizontally(float hit_nx)
{
	this->isBouncingX = true;
	this->isReturningX = false;
	this->vx = hit_nx * WOODBLOCK_BOUNCE_SPEED;
	SetState(WoodBlockState::BOUNCING);
}

void WoodBlock::HitVertically()
{
	this->isBouncingX = false;
	SetState(WoodBlockState::BOUNCING);
}