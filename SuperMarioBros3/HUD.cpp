#include "HUD.h"
#include "GameManager.h"
#include "Sprites.h"
#include "Game.h"
#include "PlayScene.h"


HUD::HUD()
{
	frameCount = 0;
	x = 0;
	y = 0;
}

Mario* GetMarioHUD() {
	return (Mario*)((PlayScene*)(SceneManager::GetInstance()->GetCurrentScene()))->GetPlayer();
}

void HUD::DrawHUD(int id, float local_x, float local_y) {
	Sprite* spr = Sprites::GetInstance()->Get(id);
	if (spr == nullptr) return;

	float top_left_x = this->x + local_x;
	float top_left_y = this->y + local_y;

	float center_x = top_left_x + spr->GetWidth() / 2.0f;
	float center_y = top_left_y + spr->GetHeight() / 2.0f;

	spr->DrawOnScreen(center_x, center_y);
}

std::string HUD::IntToString(int n, int width) {
	auto s = std::to_string(n);
	if (int(s.size()) < width) s.insert(0, width - s.size(), '0');
	return s;
}

void HUD::DrawNumber(int number, float local_x, float local_y, int width) {
	std::string s = IntToString(number, width);
	for (int i = 0; i < s.size(); ++i) {
		int id = s[i] - '0' + ID_HUD_NUMBER_0;
		DrawHUD(id, local_x + i * 8, local_y); // number width = 8px
	}
}

void HUD::DrawNumberFromRight(int number, float x, float y, int width) {
	std::string s = std::to_string(number); 

	if (s.size() < width) {
		s.insert(0, width - s.size(), ' ');
	}

	for (int i = 0; i < s.size(); ++i) {
		char c = s[s.size() - i - 1];
		int id = 0;

		if (c == ' ') {
			id = ID_HUD_NUMBER_0 + 10;
		}
		else {
			id = c - '0' + ID_HUD_NUMBER_0;
		}

		DrawHUD(id, x - i * 8, y);
	}
}

void HUD::Render()
{
	GameManager* gm = GameManager::GetInstance();

	DrawHUD(ID_HUD_PANEL, 0, 0);

	// Lấy số từ GameManager để vẽ
	DrawNumber(1, HUD_WORLD_X, HUD_WORLD_Y, 1);
	DrawNumber(gm->life, HUD_LIFE_X, HUD_LIFE_Y, 1);
	
	DrawNumber(gm->score, HUD_SCORE_X, HUD_SCORE_Y, 7);
	DrawNumber(gm->timer / 1000, HUD_TIME_X, HUD_TIME_Y, 3);
	DrawNumberFromRight(gm->coinNumber, HUD_COIN_X, HUD_COIN_Y, 2);

	float cardX[3] = { HUD_CARD_1_X, HUD_CARD_2_X, HUD_CARD_3_X };

	for (int i = 0; i < 3; i++)
	{
		bool shouldDraw = true;
		if (gm->newlyAddedCardIndex == i)
		{
			if (GetTickCount64() - gm->cardAddTimer < 2000)
			{
				if ((GetTickCount64() / 150) % 2 != 0)
				{
					shouldDraw = false;
				}
			}
			else
			{
				gm->newlyAddedCardIndex = -1;
			}
		}

		if (shouldDraw)
		{
			DrawHUD(gm->cards[i], cardX[i], HUD_CARD_Y);
		}
		else
		{
			DrawHUD(ID_HUD_CARD_NONE, cardX[i], HUD_CARD_Y);
		}
	}

	Scene* currentScene = SceneManager::GetInstance()->GetCurrentScene();
	if (dynamic_cast<PlayScene*>(currentScene))
	{
		Mario* mario = GetMarioHUD();
		if (mario != nullptr)
		{
			// Vẽ P-Meter
			for (int i = 0; i < min(mario->GetPMeter(), 6); ++i) {
				DrawHUD(ID_HUD_METER, HUD_PMETER_X + i * 8, HUD_PMETER_Y);
			}
			if (mario->GetPMeter() == 7 && frameCount < 8)
				DrawHUD(ID_HUD_METER_P, HUD_P_X, HUD_P_Y);
		}
	}

	frameCount++;
	frameCount %= 16;
}