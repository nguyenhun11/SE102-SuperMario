#include "StageNode.h"
void StageNode::Render()
{
	int spriteId = ID_SPRITE_STAGE_UNLOCK + (int)state;
	Sprites::GetInstance()->Get(spriteId)->DrawOnCamera(x, y);
}