#include "GameManager.h"
#include "PlayScene.h"
#include "Game.h"


GameManager::GameManager()
{
	score = 0;
	coinNumber = 0;
	life = 4;
	timer = 0; // 300 giây

	isGamePaused = false;
	isGameOver = false;
	isWinning = false;
	displayWinningText = false;
}

void GameManager::Update(DWORD dt)
{
	if (timer > 0) timer -= dt;
	//TODO: Cac logic khac
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

void GameManager::AddLife(int amount)	// hàm này dùng để cộng/trừ máu luôn nhee, truyền đối số âm để trừ
{
	life += amount;
	if (life < 0)
	{
		GameOver();
		return;
	}
}

void GameManager::ResetTimer(DWORD start_time)
{
	this->timer = start_time;
	this->isGameOver = false;
}

void GameManager::LevelFailed()
{
	AddLife(-1);
	isReturningFromFail = true;
	SceneManager::GetInstance()->InitiateSwitchScene(0);
}

void GameManager::LevelSuccess()
{
	mapMarioPrevX = mapMarioCurrentX;
	mapMarioPrevY = mapMarioCurrentY;

	SceneManager::GetInstance()->InitiateSwitchScene(0);
}

void GameManager::GameOver()
{
	exit(0);
}
