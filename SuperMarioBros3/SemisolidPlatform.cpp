#include "SemisolidPlatform.h"

void SemisolidPlatform::Render()
{
	NineSliceBox::Render();

	float shadowX = x + (this->cellWidth / 2) + this->columns * this->cellWidth;

	for (int i = 0; i < this->rows; i++)
	{
		float shadowY = y + (this->cellHeight / 2) + i * this->cellHeight;

		int spriteToDraw = this->shadowMid;
		if (i == 0) spriteToDraw = this->shadowTop;
		else if (i == this->rows - 1) spriteToDraw = this->shadowBot;

		Sprites::GetInstance()->Get(spriteToDraw)->Draw(shadowX, shadowY);
	}
}
