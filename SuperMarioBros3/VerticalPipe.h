#pragma once
#include "NineSliceBox.h"
class VerticalPipe : public NineSliceBox
{
private:
	int targetSceneID;

public:
	VerticalPipe(float x, float y, float cell_width, float cell_height, int rows,
		int idTopLeft, int idTopRight,
		int idBodyLeft, int idBodyRight,
		int targetSceneID = -1)
		: NineSliceBox(x, y, cell_width, cell_height, 2, rows, nullptr) 
	{
		this->spriteIDs[0][0] = idTopLeft;  
		this->spriteIDs[0][2] = idTopRight;  
		this->spriteIDs[1][0] = idBodyLeft;  
		this->spriteIDs[1][2] = idBodyRight;
		this->spriteIDs[2][0] = idBodyLeft;
		this->spriteIDs[2][2] = idBodyRight;

		this->hasTop = true;    
		this->hasBottom = false;
		this->hasLeft = true;    
		this->hasRight = true;   

		this->targetSceneID = targetSceneID;
	}

	bool isEntrance() { return targetSceneID != -1; }
	int IsBlocking() override { return 1; }

	int GetTargetSceneId() { return targetSceneID; }
};

