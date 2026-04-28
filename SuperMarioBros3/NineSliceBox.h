#pragma once
#include "GameObject.h"

//
// Hộp 9 mảnh (9-Slice/9-Patch). Thích hợp vẽ các hộp màu, khung hội thoại, 
// hoặc các bệ đỡ đất có viền cỏ xung quanh.
//
class NineSliceBox : public GameObject
{
protected:
	int columns;		// Số ô theo trục X
	int rows;			// Số ô theo trục Y
	float cellWidth;
	float cellHeight;

	// Mảng 3x3 lưu 9 SpriteID: 
	// [0][0]: Góc trên-trái    [0][1]: Cạnh trên    [0][2]: Góc trên-phải
	// [1][0]: Cạnh trái        [1][1]: Lõi giữa     [1][2]: Cạnh phải
	// [2][0]: Góc dưới-trái    [2][1]: Cạnh dưới    [2][2]: Góc dưới-phải
	int spriteIDs[3][3];

	// 4 Cờ quyết định xem có giữ lại các viền không (Mặc định là có)
	bool hasTop, hasBottom, hasLeft, hasRight;

public:
	NineSliceBox(float x, float y,
		float cell_width, float cell_height,
		int columns, int rows,
		int sprites[3][3],
		bool top = true, bool bottom = true, bool left = true, bool right = true) : GameObject(x, y)
	{
		this->cellWidth = cell_width;
		this->cellHeight = cell_height;
		this->columns = columns;
		this->rows = rows;

		if (sprites != NULL) {

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					this->spriteIDs[i][j] = sprites[i][j];
				}
			}
		}

		this->hasTop = top;
		this->hasBottom = bottom;
		this->hasLeft = left;
		this->hasRight = right;
	}

	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void RenderBoundingBox();

	int IsBlocking() { return 1; } // Khối cứng
};