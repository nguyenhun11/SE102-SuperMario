#include "NineSliceBox.h"
#include "Sprites.h"

void NineSliceBox::Render()
{
	if (this->columns <= 0 || this->rows <= 0) return;

	for (int i = 0; i < this->rows; i++)
	{
		for (int j = 0; j < this->columns; j++)
		{
			// [SỬA TẠI ĐÂY] Dịch tâm vẽ vào đúng giữa của mỗi ô lưới
			float cellX = x + (this->cellWidth / 2) + j * this->cellWidth;
			float cellY = y + (this->cellHeight / 2) + i * this->cellHeight;

			// --- KIỂM TRA TRỤC Y (DÒNG) ---
			int row_index = 1; // Khởi điểm: Luôn là Lõi giữa (Center)

			if (i == 0 && this->hasTop) row_index = 0;
			else if (i == this->rows - 1 && this->hasBottom) row_index = 2;

			// --- KIỂM TRA TRỤC X (CỘT) ---
			int col_index = 1; // Khởi điểm: Luôn là Lõi giữa (Center)

			if (j == 0 && this->hasLeft) col_index = 0;
			else if (j == this->columns - 1 && this->hasRight) col_index = 2;

			// Bốc SpriteID ra từ mảng
			int spriteToDraw = this->spriteIDs[row_index][col_index];
			Sprites::GetInstance()->Get(spriteToDraw)->Draw(cellX, cellY);
		}
	}

	// Bật dòng này lên để so sánh khung viền vật lý và hình ảnh
	// RenderBoundingBox(); 
}

void NineSliceBox::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = l + this->columns * this->cellWidth;
	b = t + this->rows * this->cellHeight;
}