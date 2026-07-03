#include "HorizontalPipe.h"
#include "PlayScene.h"
#include "SceneManager.h"
#include "Goomba.h"
#include "Mario.h"

void HorizontalPipe::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (!IsOnCamera()) return;

	if (contentType == PipeContentType::GOOMBA)
	{
		ULONGLONG now = GetTickCount64();
		if (now - lastSpawnTime > HPIPE_SPAWN_COOLDOWN)
		{
			PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
			if (scene != nullptr)
			{
				Mario* mario = (Mario*)scene->GetPlayer();
				if (mario != nullptr)
				{
					float mx, my;
					mario->GetPosition(mx, my);

					float pipeWidth = columns * cellWidth;
					bool canSpawn = false;

					float safeDistance = 16.0f;

					if (spawnSide == -1)
					{
						if (mx < (this->x - safeDistance))
						{
							canSpawn = true;
						}
					}
					else if (spawnSide == 1)
					{
						if (mx > (this->x + pipeWidth + safeDistance))
						{
							canSpawn = true;
						}
					}

					if (canSpawn)
					{
						float spawnX = this->x;
						float emergeTargetX = this->x;

						if (spawnSide == -1) // ĐẺ BÊN TRÁI ỐNG
						{
							spawnX = this->x + 16.0f;
							emergeTargetX = this->x - 8.0f;
						}
						else if (spawnSide == 1) // ĐẺ BÊN PHẢI ỐNG
						{
							spawnX = this->x + pipeWidth - 16.0f;
							emergeTargetX = this->x + pipeWidth + 8.0f;
						}

						float spawnY = this->y + 16.0f;

						Goomba* goomba = new Goomba(spawnX, spawnY);

						goomba->SetNx(this->spawnSide);
						goomba->SetSpeed(this->spawnSide * GOOMBA_WALKING_SPEED, 0.0f);
						goomba->StartEmerging(emergeTargetX);
						goomba->isGenerated = true;

						scene->AddObject(goomba);

						lastSpawnTime = now;
					}
				}
			}
		}
	}

	GameObject::Update(dt, coObjects);
}