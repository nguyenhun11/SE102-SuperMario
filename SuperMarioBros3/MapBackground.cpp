#include "MapBackground.h"
#include "GameGlobal.h"
#include "HUD.h"
#include "Sprites.h"

MapBackground::MapBackground(float x, float y, int spriteId) : GameObject(x, y)
{
	this->spriteId = spriteId;
}

void MapBackground::Render()
{
	Sprite* spr = Sprites::GetInstance()->Get(this->spriteId);
	if (spr == nullptr) return;

	// 1. Lấy kích thước không gian trống
	float screenWidth = GameGlobal::GetWidth();
	float playableHeight = GameGlobal::GetHeight() - HUD_HEIGHT; // Thường là 240 - 48 = 192px

	float mapWidth = spr->GetWidth();  // 256px
	float mapHeight = spr->GetHeight(); // 224px

	// 2. Tính tỷ lệ thu nhỏ (Scale) sao cho chiều cao Map vừa khít Playable Height
	float scale = playableHeight / mapHeight;

	// Kích thước thực tế của Map sau khi thu nhỏ
	float scaledWidth = mapWidth * scale;
	float scaledHeight = mapHeight * scale;

	// 3. Tính toán tọa độ GÓC TRÊN TRÁI để canh giữa màn hình
	float top_left_x = (screenWidth - scaledWidth) / 2.0f;
	float top_left_y = (playableHeight - scaledHeight) / 2.0f; // Sẽ = 0 vì đã scale khít chiều cao

	// 4. Chuyển từ tọa độ Góc sang Tâm (Center) cho Engine vẽ
	float center_x = top_left_x + scaledWidth / 2.0f;
	float center_y = top_left_y + scaledHeight / 2.0f;

	// 5. Vẽ ra màn hình
	// LƯU Ý: Nếu hàm Draw của class Sprite CÓ hỗ trợ truyền tỷ lệ (scale):
	// spr->Draw(center_x, center_y, scale);

	// Nếu hàm Draw chưa có tính năng scale, tạm thời vẽ canh giữa bình thường:
	spr->DrawOnScreen(center_x, center_y);
}