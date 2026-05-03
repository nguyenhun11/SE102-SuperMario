#pragma once
#include <Windows.h>
#include "Singleton.h"
class GameManager : public Singleton<GameManager>
{
	friend class Singleton<GameManager>;
	GameManager();
public:
	int score;
	int coinNumber;
	int life;
	DWORD timer;

	bool isGamePaused;
	bool isGameOver;
	bool isWinning;
	bool displayWinningText;


	void Update(DWORD dt);

	void AddCoin(int coins);
	void AddScore(int points);
};