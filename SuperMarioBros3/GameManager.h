#pragma once
#include <Windows.h>
#include "Singleton.h"
#include "HUD.h"
class GameManager : public Singleton<GameManager>
{
	friend class Singleton<GameManager>;
	GameManager();
public:
	int score;
	int coinNumber;
	int life;
	int cards[3] = { ID_HUD_CARD_NONE, ID_HUD_CARD_NONE, ID_HUD_CARD_NONE };
	DWORD timer;

	bool isGamePaused;
	bool isGameOver;
	bool isWinning;
	bool displayWinningText;


	void Update(DWORD dt);

	void AddCoin(int coins);
	void AddScore(int points);

	void ResetTimer(DWORD start_time = 300000);
};