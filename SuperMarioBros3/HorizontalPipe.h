#pragma once
#include "NineSliceBox.h"
class HorizontalPipe : public NineSliceBox
{
public:
	HorizontalPipe(float x, float y, float cell_width, float cell_height, int columns,
		int idTopLeft, int idTop, int idTopRight,
		int idBottomLeft, int idBottom,int idBottomRight)
		: NineSliceBox(x, y, cell_width, cell_height, columns, 2, nullptr)
	{

		this->spriteIDs[0][0] = idTopLeft;
		this->spriteIDs[0][1] = idTop;
		this->spriteIDs[0][2] = idTopRight;
		this->spriteIDs[2][0] = idBottomLeft;
		this->spriteIDs[2][1] = idBottom;
		this->spriteIDs[2][2] = idBottomRight;

		this->hasTop = true;
		this->hasBottom = true;
		this->hasLeft = true;
		this->hasRight = true;
	}
	int IsBlocking() override { return 1; }
};

