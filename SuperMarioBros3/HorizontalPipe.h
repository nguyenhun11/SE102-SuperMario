#pragma once
#include "NineSliceBox.h"
#include "VerticalPipe.h"

#define HPIPE_SPAWN_COOLDOWN 3000

class HorizontalPipe : public NineSliceBox
{
private:
	int targetSceneID;
	int isBlock;
	PipeContentType contentType;

	int spawnSide; // 1 phải, -1 trái
	ULONGLONG lastSpawnTime;

public:
	HorizontalPipe(float x, float y, float cell_width, float cell_height, int columns,
		int idTopLeft, int idTop, int idTopRight,
		int idBottomLeft, int idBottom, int idBottomRight,
		int isBlock = 1,
		int targetSceneID = -1,
		int pipeContentType = 0,
		int spawnSide = -1)
		: NineSliceBox(x, y, cell_width, cell_height, columns, 2, nullptr)
	{
		this->spriteIDs[0][0] = idTopLeft;
		this->spriteIDs[0][1] = idTop;
		this->spriteIDs[0][2] = idTopRight;
		this->spriteIDs[2][0] = idBottomLeft;
		this->spriteIDs[2][1] = idBottom;
		this->spriteIDs[2][2] = idBottomRight;

		this->hasTop = true;
		this->hasBottom = true;
		this->hasLeft = true;
		this->hasRight = true;

		this->targetSceneID = targetSceneID;
		this->isBlock = isBlock;
		this->contentType = (PipeContentType)pipeContentType;
		this->spawnSide = spawnSide;
		this->lastSpawnTime = 0;
	}

	int IsBlocking() override { return isBlock; }
	bool isEntrance() { return targetSceneID != -1 && targetSceneID != 999; }
	int GetTargetSceneId() { return targetSceneID; }

	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
};