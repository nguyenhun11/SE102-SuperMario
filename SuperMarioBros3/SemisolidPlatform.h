#pragma once
#include "NineSliceBox.h"
class SemisolidPlatform : public NineSliceBox
{
public:
	SemisolidPlatform(float x, float y, float cell_width, float cell_height, int columns, int rows,
		int stl, int stm, int str,
		int sml, int smm, int smr,
		int sbl, int sbm, int sbr)
		: NineSliceBox(x, y, cell_width, cell_height, columns, rows, nullptr)
	{
		this->spriteIDs[0][0] = stl; this->spriteIDs[0][1] = stm; this->spriteIDs[0][2] = str;
		this->spriteIDs[1][0] = sml; this->spriteIDs[1][1] = smm; this->spriteIDs[1][2] = smr;
		this->spriteIDs[2][0] = sbl; this->spriteIDs[2][1] = sbm; this->spriteIDs[2][2] = sbr;

		this->hasTop = true; this->hasBottom = true;
		this->hasLeft = true; this->hasRight = true;
	}

	int IsBlocking() override { return 1; }

	int IsDirectionColliable(float nx, float ny) override
	{
		if (nx == 0.0f && ny < 0.0f) return 1;

		return 0; 
	}

	//void GetBoundingBox(float& l, float& t, float& r, float& b) override
	//{
	//	l = x;
	//	t = y;
	//	r = l + this->columns * this->cellWidth;

	//	// ĐIỂM MẤU CHỐT: Không nhân với this->rows. 
	//	// Chỉ lấy độ dày bằng đúng 1 ô đầu (cellHeight) để làm bề mặt ván gỗ.
	//	b = t + this->cellHeight;
	//}
};

