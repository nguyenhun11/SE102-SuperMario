#pragma once
#include <string>
#include "Singleton.h"

#define ID_HUD_PANEL		90100
#define ID_HUD_NUMBER_0	9010100
#define ID_HUD_METER	9010200
#define ID_HUD_METER_P	9010201
#define ID_HUD_CARD_NONE		9010300
#define ID_HUD_CARD_MUSHROOM	9010301
#define ID_HUD_CARD_FLOWER		9010302
#define ID_HUD_CARD_STAR		9010303
// #define ID_UI_PAUSE_TEXT 90200
//#define ID_UI_CLEAR 1004
//#define ID_UI_YOU_GOT 1005

// Tọa độ tương đối của các thành phần trên HUD (so với góc trái trên của Panel)
#define HUD_WORLD_X 48
#define HUD_WORLD_Y 10

#define HUD_LIFE_X 48
#define HUD_LIFE_Y 18

#define HUD_SCORE_X 64
#define HUD_SCORE_Y 18

#define HUD_TIME_X 136
#define HUD_TIME_Y 18

#define HUD_COIN_X 152
#define HUD_COIN_Y 10

#define HUD_PMETER_X 64
#define HUD_PMETER_Y 10

#define HUD_P_X 112
#define HUD_P_Y 10

#define HUD_CARD_1_X 176
#define HUD_CARD_2_X 200
#define HUD_CARD_3_X 224
#define HUD_CARD_Y 10

class HUD : public Singleton<HUD>
{
	friend class Singleton<HUD>;
private:
	float x, y;
	int frameCount;
	HUD();

	void DrawHUD(int id, float x, float y);
	std::string IntToString(int n, int width = 8);
	void DrawNumber(int number, float x, float y, int width);
	void DrawNumberFromRight(int number, float x, float y, int width = 2);

public:
	void SetPosition(float x, float y) { this->x = x; this->y = y; }
	void Render();
};