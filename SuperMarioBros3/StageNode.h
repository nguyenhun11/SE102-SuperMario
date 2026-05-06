#pragma once
#include "Sprites.h"
#include "MapNode.h"

enum class StageNodeState {
	unlocked = 0,
	stage1 = 1,
	stage2 = 2,
	stage3 = 3,
	stage4 = 4,
	stage5 = 5,
	stage6 = 6
};

#define ID_SPRITE_STAGE_UNLOCK 8030100

class StageNode : public MapNode
{
private:
	StageNodeState state;

public:
	StageNode(float x, float y, int l, int r, int u, int d, int stage)
		: MapNode(x, y, l, r, u, d)
	{
		SetState((StageNodeState)stage);
	}

	void Render();

	StageNodeState GetState() { return state; }
	void SetState(StageNodeState state) { this->state = state; }

	bool IsUnlocked() { return state == StageNodeState::unlocked; }
};