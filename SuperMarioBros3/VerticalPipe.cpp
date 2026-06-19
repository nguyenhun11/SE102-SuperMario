#include "VerticalPipe.h"
#include "PlayScene.h"

void VerticalPipe::SpawnContent()
{
	if (contentType == PipeContentType::EMPTY) return;

	PlayScene* scene = (PlayScene*)SceneManager::GetInstance()->GetCurrentScene();
	if (!scene) return;

	float pipeWidth = 2 * cellWidth;
	float centerX = this->x + 8.0f;
	float topY = this->y + 8.0f;

	if (contentType == PipeContentType::PIRANHA_RED_FIRE)
	{
		PiranhaPlant* plant = new PiranhaPlant(centerX, topY,
			PIRANHA_TYPE_RED_FIRE, this->contentHeight);
		scene->AddObject(plant);
	}
	else if (contentType == PipeContentType::PIRANHA_GREEN_NORMAL)
	{
		PiranhaPlant* plant = new PiranhaPlant(centerX, topY,
			PIRANHA_TYPE_GREEN_NORMAL, this->contentHeight);
		scene->AddObject(plant);
	}
	else if (contentType == PipeContentType::PIRANHA_GREEN_FIRE)
	{
		PiranhaPlant* plant = new PiranhaPlant(centerX, topY,
			PIRANHA_TYPE_GREEN_FIRE, this->contentHeight);
		scene->AddObject(plant);
	}
	else if (contentType == PipeContentType::GOOMBA) {
		Goomba* goomba = new Goomba(centerX, topY + 16.0f);
		scene->AddObject(goomba);
	}

}