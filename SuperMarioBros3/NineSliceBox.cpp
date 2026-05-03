#include "NineSliceBox.h"
#include "Sprites.h"

void NineSliceBox::Render()
{
	if (this->columns <= 0 || this->rows <= 0) return;

	for (int i = 0; i < this->rows; i++)
	{
		for (int j = 0; j < this->columns; j++)
		{
			// (x, y) đã là tâm của ô đầu tiên. 
			// Chỉ cần dịch chuyển sang phải (j) và dịch xuống dưới (i) theo kích thước ô.
			float cellX = x + j * this->cellWidth;
			float cellY = y + i * this->cellHeight;

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
}

void NineSliceBox::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	// Lùi (x,y) về nửa ô để lấy đúng mép ngoài cùng (Top-Left) của khối
	l = x - (this->cellWidth / 2.0f);
	t = y - (this->cellHeight / 2.0f);

	// Có góc Top-Left rồi thì chỉ cần cộng thêm tổng chiều dài và tổng chiều cao
	r = l + (this->columns * this->cellWidth);
	b = t + (this->rows * this->cellHeight);
}