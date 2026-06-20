#pragma once
#include "NineSliceBox.h"
#include "AssetIDs.h"
#include "PiranhaPlant.h"
enum class PipeContentType {
	EMPTY = 0,
	PIRANHA_RED_FIRE		= OBJECT_TYPE_PIRANHA_PLANT + PIRANHA_TYPE_RED_FIRE,
	PIRANHA_GREEN_NORMAL	= OBJECT_TYPE_PIRANHA_PLANT + PIRANHA_TYPE_GREEN_NORMAL,
	PIRANHA_GREEN_FIRE		= OBJECT_TYPE_PIRANHA_PLANT + PIRANHA_TYPE_GREEN_FIRE,
	GOOMBA = OBJECT_TYPE_GOOMBA,
};

class VerticalPipe : public NineSliceBox
{
private:
	int targetSceneID;
	int isBlock;
	PipeContentType contentType;
	float contentHeight;

public:
	VerticalPipe(float x, float y, float cell_width, float cell_height, int rows,
		int idTopLeft, int idTopRight,
		int idBodyLeft, int idBodyRight,
		int idBottomLeft, int idBottomRight, int isBlock = 1,
		int targetSceneID = -1,
		int pipeContentType = 0,
		float contentHeight = 32.0f)
		: NineSliceBox(x, y, cell_width, cell_height, 2, rows, nullptr, true, true, true, true, 11)
	{
		this->spriteIDs[0][0] = idTopLeft;
		this->spriteIDs[1][0] = idBodyLeft;
		this->spriteIDs[2][0] = idBottomLeft;
		this->spriteIDs[0][2] = idTopRight;
		this->spriteIDs[1][2] = idBodyRight;
		this->spriteIDs[2][2] = idBottomRight;

		this->hasTop = true;
		this->hasBottom = true;
		this->hasLeft = true;
		this->hasRight = true;

		this->targetSceneID = targetSceneID;
		this->isBlock = isBlock;
		this->contentType = (PipeContentType)pipeContentType;
		this->contentHeight = contentHeight;

		zIndex = 9;
	}

	bool isEntrance() { return targetSceneID != -1 && targetSceneID != 999; }
	int IsBlocking() override { return isBlock; }

	int GetTargetSceneId() { return targetSceneID; }
	void SpawnContent();
};


