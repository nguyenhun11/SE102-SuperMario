#include "GameManager.h"
#include "PlayScene.h"
#include "Game.h"


GameManager::GameManager()
{
	score = 0;
	coinNumber = 0;
	life = 4;
	timer = 300000; // 300 giây

	isGamePaused = false;
	isGameOver = false;
	isWinning = false;
	displayWinningText = false;
}

void GameManager::Update(DWORD dt)
{
	if (timer > 0) timer -= dt;
}

void GameManager::AddCoin(int coins)
{
	coinNumber += coins;
	if (coinNumber >= 100)
	{
		life++;
		coinNumber = 0;
	}
}

void GameManager::AddScore(int points)
{
	score += points;
}