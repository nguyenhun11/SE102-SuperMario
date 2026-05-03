#pragma once
#include "NineSliceBox.h"

class Ground : public NineSliceBox
{
public:
	Ground(float x, float y, 
		float cell_width, float cell_height, 
		int columns, int rows,
		int sID_tl, int sID_tm, int sID_tr,
		int sID_ml, int sID_mm, int sID_mr
	)
		: NineSliceBox(x, y, cell_width, cell_height, columns, rows, nullptr) // Truyền tạm nullptr
	{
		this->spriteIDs[0][0] = sID_tl; this->spriteIDs[0][1] = sID_tm; this->spriteIDs[0][2] = sID_tr;
		this->spriteIDs[1][0] = sID_ml; this->spriteIDs[1][1] = sID_mm; this->spriteIDs[1][2] = sID_mr;
		this->spriteIDs[2][0] = sID_ml; this->spriteIDs[2][1] = sID_mm; this->spriteIDs[2][2] = sID_mr;

		// Mặc định Ground là mặt đất nền, nên ta bật hết các viền
		this->hasTop = true;
		this->hasBottom = false;
		this->hasLeft = true;
		this->hasRight = true;
	}

	void SetNoBottom() { this->hasBottom = false; }
};