#pragma once
#include <Windows.h>
#include "Singleton.h"
#include "HUD.h"


class GameManager : public Singleton<GameManager>
{
	friend class Singleton<GameManager>;
	GameManager();
public:
	float mapMarioCurrentX = -1.0f;
	float mapMarioCurrentY = -1.0f;

	float mapMarioPrevX = -1.0f;
	float mapMarioPrevY = -1.0f;

	int marioForm;

	bool isReturningFromFail = false;
	bool isGoingThroughPipe = false;

	int score;
	int coinNumber;
	int life;
	int cards[3] = { ID_HUD_CARD_NONE, ID_HUD_CARD_NONE, ID_HUD_CARD_NONE };
	int newlyAddedCardIndex = -1;
	ULONGLONG cardAddTimer = 0;  
	DWORD timer;

	bool isGamePaused;
	bool isGameOver;
	bool isWinning;
	bool displayWinningText;


	void Update(DWORD dt);

	void AddCoin(int coins = 1);
	void AddScore(int points = 100);
	void AddLife(int amount = 1);
	void AddCard(int hudCardId);

	void ResetTimer(DWORD start_time = 300000);

	void LevelFailed();
	void LevelSuccess();
	void GameOver();
};