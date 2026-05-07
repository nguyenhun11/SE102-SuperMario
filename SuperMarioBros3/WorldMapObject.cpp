#include "WorldMapObject.h"
#include "Animations.h"

WorldMapObject::WorldMapObject(float x, float y, int aniId) : GameObject(x, y)
{
	this->aniId = aniId;
}

void WorldMapObject::Render()
{
	Animations::GetInstance()->Get(aniId)->Render(x, y);
}