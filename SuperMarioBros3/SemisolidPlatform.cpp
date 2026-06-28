#include "SemisolidPlatform.h"

void SemisolidPlatform::Render()
{
	NineSliceBox::Render();

	float shadowX = x + this->columns * this->cellWidth;

	for (int i = 0; i < this->rows; i++)
	{
		float shadowY = y + i * this->cellHeight;

		int spriteToDraw = this->shadowMid;
		if (i == 0) spriteToDraw = this->shadowTop;
		else if (i == this->rows - 1) spriteToDraw = this->shadowBot;

		Sprites::GetInstance()->Get(spriteToDraw)->DrawOnCamera(shadowX, shadowY);
	}

	if (shadowTop_Bottom != 0 && shadowMid_Bottom != 0 && shadowBot_Bottom != 0)
	{
		float shadowY_Bot = y + this->rows * this->cellHeight;

		for (int i = 0; i < this->columns + 1; i++)
		{
			float shadowX_Bot = x + i * this->cellWidth;

			int spriteToDraw = this->shadowMid_Bottom;
			if (i == 0) spriteToDraw = this->shadowTop_Bottom;
			else if (i == this->columns) spriteToDraw = this->shadowBot_Bottom;

			Sprites::GetInstance()->Get(spriteToDraw)->DrawOnCamera(shadowX_Bot, shadowY_Bot);
		}
	}
}
