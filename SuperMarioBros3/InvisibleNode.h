#pragma once
#include "MapNode.h"
class InvisibleNode : public MapNode
{
	public:
	InvisibleNode(float x, float y, int l, int r, int u, int d) : MapNode(x, y, l, r, u, d) {
		zIndex = 5;
	}
	virtual void Render() {
		RenderBoundingBox();
	}
};

