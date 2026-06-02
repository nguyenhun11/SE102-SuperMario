#pragma once
#include "NineSliceBox.h"
class VerticalPipe : public NineSliceBox
{
private:
	int targetSceneID;
	int isBlock;

public:
	VerticalPipe(float x, float y, float cell_width, float cell_height, int rows,
		int idTopLeft, int idTopRight,
		int idBodyLeft, int idBodyRight,
		int idBottomLeft, int idBottomRight, int isBlock = 1,
		int targetSceneID = -1)
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
	}

	bool isEntrance() { return targetSceneID != -1 && targetSceneID != 999; }
	int IsBlocking() override { return isBlock; }

	int GetTargetSceneId() { return targetSceneID; }
};

